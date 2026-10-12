#ifndef PERSISTENCIA_JSON_H
#define PERSISTENCIA_JSON_H

#include <string>
#include "ArregloDinamico.h"
#include "ArbolBinarioPeliculas.h"
#include "ArbolAVLFunciones.h"
#include "ArbolBClientes.h"
#include "TablaHashReservas.h"
#include "GrafoSedes.h"

// Resultado de una operacion de carga masiva. "errores" es un
// ArregloDinamico (arreglo dinamico propio, no std::vector) con el
// detalle de cada registro rechazado, para mostrarselo al
// administrador en la interfaz.
struct ResultadoCarga {
    int exitosos = 0;
    int rechazados = 0;
    ArregloDinamico<std::string> errores;

    bool huboErrorFatal = false;
};

class PersistenciaJSON {
public:
    // A partir de Fase 3, el archivo tambien trae un arreglo
    // "sedes" (que se cargan como vertices del grafo ANTES que las
    // peliculas/funciones), y cada funcion debe referenciar una
    // sede existente mediante su campo "sede" -- si no, la funcion
    // completa se rechaza (enunciado 4.1, "Carga masiva").
    static ResultadoCarga cargarPeliculasYFunciones(
        const std::string& rutaArchivo,
        ArbolBinarioPeliculas& arbolPeliculas,
        ArbolAVLFunciones& arbolFunciones,
        GrafoSedes& grafo);

    static ResultadoCarga cargarClientesYReservas(
        const std::string& rutaArchivo,
        ArbolBClientes& arbolClientes,
        TablaHashReservas& tablaReservas);

    static bool esFechaValida(const std::string& fecha);

    static std::string fechaHoy();

private:
    static bool campoTextoValido(const std::string& valor);
};

#endif // PERSISTENCIA_JSON_H
