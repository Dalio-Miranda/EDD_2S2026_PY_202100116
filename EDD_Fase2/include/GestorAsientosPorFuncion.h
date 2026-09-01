#ifndef GESTOR_ASIENTOS_POR_FUNCION_H
#define GESTOR_ASIENTOS_POR_FUNCION_H

#include <string>
#include "MatrizDispersaAsientos.h"
#include "ArbolAVLFunciones.h"

// Orquesta el ciclo de vida de la Matriz Dispersa en Fase 2: en vez de
// existir una unica funcion fija (como en Fase 1), ahora la matriz se
// carga y vacia segun la funcion que el administrador/cliente tenga
// seleccionada, persistiendo cada funcion en su propio archivo
// F00X_funcion.json (enunciado 4.1, puntos 01 y 03).
//
// IMPORTANTE: esta clase NO modifica MatrizDispersaAsientos (la
// implementacion interna de Fase 1 se reutiliza tal cual, como pide
// el enunciado en "Consideraciones de Migracion"). El campo
// "nombreCliente" de cada NodoAsiento se reutiliza aqui para guardar
// el codigo_reserva -- es el mismo campo generico tipo string, solo
// que en el flujo de Fase 2 su contenido semantico es un
// codigo_reserva en vez de un nombre. Se documenta explicitamente en
// cada punto de uso para evitar confusion.
class GestorAsientosPorFuncion {
private:
    MatrizDispersaAsientos matrizActiva;
    std::string codigoFuncionActiva; // "" si no hay ninguna funcion cargada
    std::string carpetaAsientos;     // carpeta donde viven los F00X_funcion.json

    std::string rutaArchivo(const std::string& codigoFuncion) const;

    // Vuelca los asientos ocupados actuales de matrizActiva a su
    // archivo JSON (codigo_funcion + asientos_ocupados[fila,columna,
    // codigo_reserva]). No hace nada si no hay funcion activa.
    void escribirArchivoActiva() const;

    // Lee (si existe) el archivo F00X_funcion.json de esa funcion y
    // reserva en matrizActiva cada asiento ocupado alli guardado.
    // Si el archivo no existe todavia (funcion recien creada, sin
    // reservas), no es un error: la matriz simplemente queda vacia.
    void cargarDesdeArchivo(const std::string& codigoFuncion);

public:
    explicit GestorAsientosPorFuncion(std::string carpetaAsientos = "data/asientos");

    GestorAsientosPorFuncion(const GestorAsientosPorFuncion&) = delete;
    GestorAsientosPorFuncion& operator=(const GestorAsientosPorFuncion&) = delete;

    // Carga la funcion identificada por codigoFuncion como la activa.
    // Si habia otra funcion activa, la persiste primero y vacia la
    // matriz antes de cargar la nueva (enunciado 4.1, punto 03,
    // "Cargar una funcion especifica con matriz activa"). Retorna
    // false si el codigo no existe en el Arbol AVL de funciones.
    bool cargarFuncion(const std::string& codigoFuncion, ArbolAVLFunciones& arbolFunciones);

    // Persiste el estado actual de la matriz activa a su archivo.
    // No hace nada si no hay ninguna funcion cargada.
    void persistirActiva() const;

    // Descarga la funcion activa (persistiendo primero) y deja el
    // gestor sin ninguna funcion cargada.
    void descargarActiva();

    bool hayFuncionActiva() const;
    std::string getCodigoFuncionActiva() const;

    // Acceso directo a la matriz activa (para mostrar el mapa de
    // asientos en la UI o generar el reporte Graphviz de la funcion
    // actualmente cargada).
    MatrizDispersaAsientos& getMatrizActiva();

    // Reserva/cancela un asiento en la funcion ACTIVA, persistiendo
    // automaticamente al archivo despues de cada cambio (enunciado
    // 4.1, punto 03: "actualiza la matriz en memoria, la persiste de
    // vuelta al archivo de esa funcion"). Retorna false si no hay
    // funcion activa o si la operacion sobre la matriz falla
    // (fuera de rango / ya ocupado / no existia la reserva).
    bool reservarAsiento(int fila, int columna, const std::string& codigoReserva);
    bool cancelarAsiento(int fila, int columna);

    // Elimina el archivo .json de asientos de una funcion (usado al
    // eliminar una funcion del AVL). Si esa funcion es la que esta
    // activa, se descarga primero SIN persistir (no tiene sentido
    // guardar lo que se va a borrar de inmediato).
    bool eliminarArchivoFuncion(const std::string& codigoFuncion);
};

#endif // GESTOR_ASIENTOS_POR_FUNCION_H
