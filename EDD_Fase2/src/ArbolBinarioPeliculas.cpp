#include "ArbolBinarioPeliculas.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

ArbolBinarioPeliculas::ArbolBinarioPeliculas()
    : raiz(nullptr), tamanio(0) {}

ArbolBinarioPeliculas::~ArbolBinarioPeliculas() {
    liberar(raiz);
    raiz = nullptr;
}

void ArbolBinarioPeliculas::liberar(NodoArbolPelicula* nodo) {
    if (nodo == nullptr) return;
    liberar(nodo->izquierdo);
    liberar(nodo->derecho);
    delete nodo;
}

void ArbolBinarioPeliculas::insertar(const Pelicula& p) {
    bool insertado = false;
    raiz = insertarRec(raiz, p, insertado);
    if (insertado) tamanio++;
}

NodoArbolPelicula* ArbolBinarioPeliculas::insertarRec(NodoArbolPelicula* nodo, const Pelicula& p, bool& insertado) {
    if (nodo == nullptr) {
        insertado = true;
        return new NodoArbolPelicula(p);
    }

    if (p.codigo < nodo->dato.codigo) {
        nodo->izquierdo = insertarRec(nodo->izquierdo, p, insertado);
    } else if (p.codigo > nodo->dato.codigo) {
        nodo->derecho = insertarRec(nodo->derecho, p, insertado);
    } else {
        // Codigo ya existente: actualizamos los datos en vez de duplicar.
        nodo->dato = p;
        insertado = false;
    }

    return nodo;
}

Pelicula* ArbolBinarioPeliculas::buscar(const std::string& codigo) {
    return buscarRec(raiz, codigo);
}

Pelicula* ArbolBinarioPeliculas::buscarRec(NodoArbolPelicula* nodo, const std::string& codigo) const {
    if (nodo == nullptr) return nullptr;

    if (codigo == nodo->dato.codigo) {
        return &(nodo->dato);
    } else if (codigo < nodo->dato.codigo) {
        return buscarRec(nodo->izquierdo, codigo);
    } else {
        return buscarRec(nodo->derecho, codigo);
    }
}

NodoArbolPelicula* ArbolBinarioPeliculas::encontrarMinimo(NodoArbolPelicula* nodo) const {
    while (nodo->izquierdo != nullptr) {
        nodo = nodo->izquierdo;
    }
    return nodo;
}

bool ArbolBinarioPeliculas::eliminar(const std::string& codigo) {
    bool eliminado = false;
    raiz = eliminarRec(raiz, codigo, eliminado);
    if (eliminado) tamanio--;
    return eliminado;
}

NodoArbolPelicula* ArbolBinarioPeliculas::eliminarRec(NodoArbolPelicula* nodo, const std::string& codigo, bool& eliminado) {
    if (nodo == nullptr) {
        return nullptr; // no se encontro
    }

    if (codigo < nodo->dato.codigo) {
        nodo->izquierdo = eliminarRec(nodo->izquierdo, codigo, eliminado);
    } else if (codigo > nodo->dato.codigo) {
        nodo->derecho = eliminarRec(nodo->derecho, codigo, eliminado);
    } else {
        // Encontramos el nodo a eliminar. Tres casos posibles:
        eliminado = true;

        if (nodo->izquierdo == nullptr && nodo->derecho == nullptr) {
            // Caso 1: nodo hoja, sin hijos.
            delete nodo;
            return nullptr;
        } else if (nodo->izquierdo == nullptr) {
            // Caso 2: un solo hijo (derecho).
            NodoArbolPelicula* temp = nodo->derecho;
            delete nodo;
            return temp;
        } else if (nodo->derecho == nullptr) {
            // Caso 2: un solo hijo (izquierdo).
            NodoArbolPelicula* temp = nodo->izquierdo;
            delete nodo;
            return temp;
        } else {
            // Caso 3: dos hijos. Buscamos el sucesor inorden
            // (el minimo del subarbol derecho), copiamos su dato
            // en este nodo, y luego eliminamos ese sucesor de su
            // posicion original (que siempre cae en un caso 1 o 2).
            NodoArbolPelicula* sucesor = encontrarMinimo(nodo->derecho);
            nodo->dato = sucesor->dato;

            bool ignorado = false; // ya contamos "eliminado" arriba
            nodo->derecho = eliminarRec(nodo->derecho, sucesor->dato.codigo, ignorado);
        }
    }

    return nodo;
}

// Separa una linea de CSV por comas en un vector de campos.
static std::vector<std::string> separarLinea(const std::string& linea, char delimitador) {
    std::vector<std::string> campos;
    std::stringstream ss(linea);
    std::string campo;

    while (std::getline(ss, campo, delimitador)) {
        campos.push_back(campo);
    }
    return campos;
}

int ArbolBinarioPeliculas::cargarDesdeCSV(const std::string& rutaArchivo) {
    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        std::cerr << "No se pudo abrir el archivo: " << rutaArchivo << std::endl;
        return 0;
    }

    std::string linea;
    int contador = 0;
    bool primeraLinea = true;

    while (std::getline(archivo, linea)) {
        if (linea.empty()) continue;

        if (primeraLinea) {
            primeraLinea = false;
            continue; // saltamos el encabezado
        }

        std::vector<std::string> campos = separarLinea(linea, ',');

        if (campos.size() < 8) {
            std::cerr << "Linea CSV invalida (se ignora): " << linea << std::endl;
            continue;
        }

        Pelicula p;
        p.codigo         = campos[0];
        p.titulo         = campos[1];
        p.genero         = campos[2];
        p.duracion       = std::stoi(campos[3]);
        p.clasificacion  = campos[4];
        p.idioma         = campos[5];
        p.fechaEstreno   = campos[6];
        p.fechaFin       = campos[7];

        insertar(p);
        contador++;
    }

    archivo.close();
    return contador;
}

void ArbolBinarioPeliculas::imprimirInorden() const {
    if (raiz == nullptr) {
        std::cout << "La cartelera esta vacia." << std::endl;
        return;
    }
    std::cout << "===== Cartelera (" << tamanio << " peliculas, recorrido inorden) =====" << std::endl;
    imprimirInordenRec(raiz);
}

void ArbolBinarioPeliculas::imprimirInordenRec(NodoArbolPelicula* nodo) const {
    if (nodo == nullptr) return;

    imprimirInordenRec(nodo->izquierdo);

    const Pelicula& p = nodo->dato;
    std::cout << p.codigo << " | " << p.titulo
               << " | " << p.genero
               << " | " << p.duracion << " min"
               << " | " << p.clasificacion
               << " | " << p.idioma
               << " | Estreno: " << p.fechaEstreno
               << " | Fin: " << p.fechaFin
               << std::endl;

    imprimirInordenRec(nodo->derecho);
}

int ArbolBinarioPeliculas::getTamanio() const {
    return tamanio;
}

bool ArbolBinarioPeliculas::estaVacia() const {
    return tamanio == 0;
}

NodoArbolPelicula* ArbolBinarioPeliculas::getRaiz() const {
    return raiz;
}