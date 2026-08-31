#ifndef FUNCION_H
#define FUNCION_H

#include <string>
#include <nlohmann/json.hpp>

// Representa una funcion de cine. Se indexa en el ArbolAVLFunciones
// usando codigoFuncion como clave.
//
// archivoAsientos empieza vacio ("") en la carga masiva y se completa
// con el nombre del archivo (ej. "F001_funcion.json") en el momento
// en que la funcion se crea desde la interfaz (ver enunciado 4.1,
// punto 01 y 03). Ese archivo es el que despues alimenta la matriz
// dispersa cuando la funcion se selecciona/activa.
struct Funcion {
    std::string codigoFuncion;
    std::string codigoPelicula;   // FK logica hacia el BST de peliculas
    std::string horario;
    std::string sala;
    int filas = 0;
    int columnas = 0;
    std::string archivoAsientos;  // "" hasta que se crea la funcion en el sistema

    // true si la funcion ya se realizo (para el color rojo del Reporte 2:
    // AVL de Funciones). Se decide comparando fecha/hora actual vs horario;
    // la logica de esa comparacion vive en Validaciones, no aqui.
    bool yaRealizada = false;

    Funcion() = default;

    Funcion(std::string codigoFuncion,
            std::string codigoPelicula,
            std::string horario,
            std::string sala,
            int filas,
            int columnas,
            std::string archivoAsientos = "")
        : codigoFuncion(std::move(codigoFuncion)),
          codigoPelicula(std::move(codigoPelicula)),
          horario(std::move(horario)),
          sala(std::move(sala)),
          filas(filas),
          columnas(columnas),
          archivoAsientos(std::move(archivoAsientos)) {}

    // Nombre de archivo estandar para una funcion, ej. "F001_funcion.json".
    // Centralizado aqui para que el AVL, la persistencia y los paneles
    // de UI generen siempre el mismo nombre y no se desincronicen.
    static std::string nombreArchivoPara(const std::string& codigoFuncion) {
        return codigoFuncion + "_funcion.json";
    }
};

// ---------------------------------------------------------------------
// Serializacion JSON (nlohmann/json). Estas funciones son las que usa
// PersistenciaJSON para la carga masiva de peliculas+funciones y para
// guardar/leer el nodo de funcion dentro del AVL cuando se exporta el
// catalogo completo (no confundir con el archivo F00X_funcion.json de
// asientos ocupados, que es un formato distinto -- ver Funcion.cpp).
// ---------------------------------------------------------------------
void to_json(nlohmann::json& j, const Funcion& f);
void from_json(const nlohmann::json& j, Funcion& f);

#endif // FUNCION_H
