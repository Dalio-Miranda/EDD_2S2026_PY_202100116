#include "PersistenciaEDD.h"
#include "LZW.h"
#include "Sede.h"
#include "Funcion.h"
#include "Pelicula.h"
#include "TipoRecorrido.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iterator>

using nlohmann::json;

// ---------------------------------------------------------------
// Utilidades de archivo binario <-> ArregloDinamico<unsigned char>
// ---------------------------------------------------------------

bool PersistenciaEDD::escribirComprimido(const std::string& ruta, const std::string& texto) {
    ArregloDinamico<unsigned char> bytes = LZW::comprimir(texto);
    std::ofstream archivo(ruta, std::ios::binary | std::ios::trunc);
    if (!archivo.is_open()) return false;
    for (int i = 0; i < bytes.getTamanio(); i++) {
        archivo.put(static_cast<char>(bytes[i]));
    }
    return archivo.good();
}

bool PersistenciaEDD::leerDescomprimido(const std::string& ruta, std::string& textoSalida) {
    std::ifstream archivo(ruta, std::ios::binary);
    if (!archivo.is_open()) return false;

    ArregloDinamico<unsigned char> bytes;
    char c;
    while (archivo.get(c)) {
        bytes.agregarFinal(static_cast<unsigned char>(c));
    }
    // LZW usa codigos de 2 bytes: un tamanio impar indica archivo danado.
    if (bytes.getTamanio() % 2 != 0) return false;

    textoSalida = LZW::descomprimir(bytes);
    return true;
}

// ---------------------------------------------------------------
// Serializacion: se recorre la estructura y se arma el texto en RAM.
// ---------------------------------------------------------------

std::string PersistenciaEDD::serializarSedes(const GrafoSedes& grafo) {
    json arreglo = json::array();
    // Recorre cada vertice del grafo (cada sede) directamente.
    for (int i = 0; i < grafo.getCantidadVertices(); i++) {
        json js;
        to_json(js, grafo.getVertice(i)->dato);
        arreglo.push_back(js);
    }
    json raiz;
    raiz["sedes"] = arreglo;
    return raiz.dump(2);
}

std::string PersistenciaEDD::serializarPeliculas(const ArbolBinarioPeliculas& peliculas) {
    json arreglo = json::array();
    ArregloDinamico<Pelicula> lista = peliculas.listarPeliculas(TipoRecorrido::PREORDEN);
    // Preorden: al reinsertar en este orden el arbol conserva su forma.
    for (int i = 0; i < lista.getTamanio(); i++) {
        const Pelicula& p = lista[i];
        json jp;
        jp["codigo"] = p.codigo;
        jp["titulo"] = p.titulo;
        jp["genero"] = p.genero;
        jp["duracion"] = p.duracion;
        jp["clasificacion"] = p.clasificacion;
        jp["idioma"] = p.idioma;
        jp["fecha_estreno"] = p.fechaEstreno;
        jp["fecha_fin"] = p.fechaFin;
        arreglo.push_back(jp);
    }
    json raiz;
    raiz["peliculas"] = arreglo;
    return raiz.dump(2);
}

std::string PersistenciaEDD::serializarFunciones(const ArbolAVLFunciones& funciones) {
    json arreglo = json::array();
    ArregloDinamico<Funcion> lista = funciones.listarFunciones(TipoRecorrido::PREORDEN);
    for (int i = 0; i < lista.getTamanio(); i++) {
        json jf;
        to_json(jf, lista[i]);
        // El JSON de una Funcion NO incluye su pelicula (en la carga masiva
        // la funcion viene anidada dentro de la pelicula), asi que aqui se
        // guarda explicitamente: sin este campo, al reabrir el programa las
        // funciones quedarian sin pelicula y el grafo se calcularia mal.
        jf["codigo_pelicula"] = lista[i].codigoPelicula;
        arreglo.push_back(jf);
    }
    json raiz;
    raiz["funciones"] = arreglo;
    return raiz.dump(2);
}

bool PersistenciaEDD::guardarTodo(const std::string& carpeta,
                                  const GrafoSedes& grafo,
                                  const ArbolBinarioPeliculas& peliculas,
                                  const ArbolAVLFunciones& funciones) {
    bool ok = true;
    ok = escribirComprimido(carpeta + "/sedes.edd", serializarSedes(grafo)) && ok;
    ok = escribirComprimido(carpeta + "/peliculas.edd", serializarPeliculas(peliculas)) && ok;
    ok = escribirComprimido(carpeta + "/funciones.edd", serializarFunciones(funciones)) && ok;
    return ok;
}

// ---------------------------------------------------------------
// Carga: descomprimir y reconstruir.
// ---------------------------------------------------------------

bool PersistenciaEDD::cargarTodo(const std::string& carpeta,
                                 GrafoSedes& grafo,
                                 ArbolBinarioPeliculas& peliculas,
                                 ArbolAVLFunciones& funciones) {
    std::string textoSedes, textoPeliculas, textoFunciones;
    if (!leerDescomprimido(carpeta + "/sedes.edd", textoSedes)) return false;
    if (!leerDescomprimido(carpeta + "/peliculas.edd", textoPeliculas)) return false;
    if (!leerDescomprimido(carpeta + "/funciones.edd", textoFunciones)) return false;

    // Se parsea TODO primero; si algo esta danado, no se toca ninguna
    // estructura (carga atomica).
    json jSedes, jPeliculas, jFunciones;
    try {
        jSedes = json::parse(textoSedes);
        jPeliculas = json::parse(textoPeliculas);
        jFunciones = json::parse(textoFunciones);
        if (!jSedes.at("sedes").is_array() || !jPeliculas.at("peliculas").is_array() ||
            !jFunciones.at("funciones").is_array()) return false;
    } catch (const std::exception&) {
        return false;
    }

    // 1) Sedes (vertices del grafo).
    for (const json& js : jSedes["sedes"]) {
        try {
            Sede s;
            from_json(js, s);
            grafo.agregarSede(s);
        } catch (const std::exception&) { /* registro danado: se omite */ }
    }

    // 2) Peliculas.
    for (const json& jp : jPeliculas["peliculas"]) {
        try {
            Pelicula p;
            p.codigo = jp.at("codigo").get<std::string>();
            p.titulo = jp.at("titulo").get<std::string>();
            p.genero = jp.at("genero").get<std::string>();
            p.duracion = jp.at("duracion").get<int>();
            p.clasificacion = jp.at("clasificacion").get<std::string>();
            p.idioma = jp.at("idioma").get<std::string>();
            p.fechaEstreno = jp.at("fecha_estreno").get<std::string>();
            p.fechaFin = jp.at("fecha_fin").get<std::string>();
            peliculas.insertar(p);
        } catch (const std::exception&) { /* se omite */ }
    }

    // 3) Funciones.
    for (const json& jf : jFunciones["funciones"]) {
        try {
            Funcion f;
            from_json(jf, f);
            f.codigoPelicula = jf.at("codigo_pelicula").get<std::string>();
            funciones.insertar(f);
        } catch (const std::exception&) { /* se omite */ }
    }

    // 4) Las aristas no se guardan: se derivan de las funciones.
    for (int i = 0; i < grafo.getCantidadVertices(); i++) {
        grafo.recalcularRelacionesDeSede(grafo.getVertice(i)->dato.codigo, funciones);
    }
    return true;
}