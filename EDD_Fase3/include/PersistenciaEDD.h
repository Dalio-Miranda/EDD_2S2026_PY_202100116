#ifndef PERSISTENCIA_EDD_H
#define PERSISTENCIA_EDD_H

#include <string>
#include "ArbolBinarioPeliculas.h"
#include "ArbolAVLFunciones.h"
#include "GrafoSedes.h"

// Persistencia comprimida con LZW (Fase 3): sedes.edd, peliculas.edd
// y funciones.edd.
//
// FLUJO DE GUARDADO (al cerrar el programa):
//   estructura en memoria --recorrido--> texto serializado (std::string
//   construido en RAM) --LZW--> bytes --> archivo .edd
// NO existe ningun archivo JSON/XML intermedio: el texto JSON vive solo
// en una variable en memoria y se entrega directamente a LZW::comprimir.
//
// FLUJO DE CARGA (al iniciar): archivo .edd --> bytes --LZW--> texto
// --> reconstruccion de las estructuras.
//
// Siempre se serializa el estado ACTUAL de las estructuras; nunca se
// vuelve a consultar el archivo de carga masiva.
class PersistenciaEDD {
public:
    // Genera/sobrescribe sedes.edd, peliculas.edd y funciones.edd dentro
    // de "carpeta". Retorna true si los tres se escribieron bien.
    static bool guardarTodo(const std::string& carpeta,
                            const GrafoSedes& grafo,
                            const ArbolBinarioPeliculas& peliculas,
                            const ArbolAVLFunciones& funciones);

    // Si existen los tres archivos, los descomprime y reconstruye las
    // estructuras (sedes -> peliculas -> funciones -> aristas del
    // grafo). Retorna true si se cargo el estado guardado; false si no
    // habia archivos (primera ejecucion) o alguno estaba danado (en ese
    // caso las estructuras quedan sin modificar).
    static bool cargarTodo(const std::string& carpeta,
                           GrafoSedes& grafo,
                           ArbolBinarioPeliculas& peliculas,
                           ArbolAVLFunciones& funciones);

    // Serializadores (publicos para poder probarlos): recorren la
    // estructura y devuelven el texto estructurado en memoria.
    static std::string serializarSedes(const GrafoSedes& grafo);
    static std::string serializarPeliculas(const ArbolBinarioPeliculas& peliculas);
    static std::string serializarFunciones(const ArbolAVLFunciones& funciones);

private:
    static bool escribirComprimido(const std::string& ruta, const std::string& texto);
    static bool leerDescomprimido(const std::string& ruta, std::string& textoSalida);
};

#endif // PERSISTENCIA_EDD_H
