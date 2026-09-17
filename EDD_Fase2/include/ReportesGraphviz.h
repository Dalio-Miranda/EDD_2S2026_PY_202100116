#ifndef REPORTES_GRAPHVIZ_H
#define REPORTES_GRAPHVIZ_H

#include <string>
#include "ArbolBinarioPeliculas.h"
#include "ListaCircularSolicitudes.h"
#include "ListaCircularPromociones.h"
#include "MatrizDispersaAsientos.h"
#include "ArbolAVLFunciones.h"
#include "ArbolBClientes.h"
#include "TablaHashReservas.h"
#include "GestorAsientosPorFuncion.h"

// Genera los reportes graficos que pide el enunciado, usando
// Graphviz. Cada funcion escribe un archivo .dot en carpetaReportes
// y luego invoca el comando "dot" para renderizarlo como .png.
namespace ReportesGraphviz {

    // Reporte 1: Arbol Binario de Busqueda (Cartelera de Peliculas).
    // Verde = en cartelera, Amarillo = a menos de 7 dias de retirarse.
    void generarReporteCartelera(const ArbolBinarioPeliculas& arbol,
                                  const std::string& carpetaReportes);

    // Reporte: Lista Circular Doblemente Enlazada (Solicitudes Pendientes).
    void generarReporteSolicitudes(const ListaCircularSolicitudes& solicitudes,
                                    const std::string& carpetaReportes);

    // Reporte: Lista Circular de Listas (Promociones y Beneficios).
    void generarReportePromociones(const ListaCircularPromociones& promociones,
                                    const std::string& carpetaReportes);

    // Reporte (Fase 1): Matriz Dispersa de la unica funcion activa.
    void generarReporteMatrizAsientos(const MatrizDispersaAsientos& matriz,
                                       const std::string& carpetaReportes);

    // --------------------- Reportes nuevos de Fase 2 ---------------------

    // Reporte: Arbol AVL de Funciones. Nodos circulares; verde =
    // funcion programada, rojo = ya realizada (segun si su horario ya
    // paso hoy -- el modelo de datos no guarda una fecha propia por
    // funcion, solo un horario de hora del dia, ver enunciado 4.1).
    void generarReporteFunciones(const ArbolAVLFunciones& arbol,
                                  const std::string& carpetaReportes);

    // Reporte: Arbol B de orden 4 de Clientes. Cada nodo se dibuja
    // como una caja dividida en compartimentos (uno por clave/id),
    // con color distinto para raiz, nodos internos y hojas.
    void generarReporteClientes(const ArbolBClientes& arbol,
                                 const std::string& carpetaReportes);

    // Reporte: Tabla Hash de Reservas. Buckets numerados; las
    // colisiones se muestran como listas enlazadas dentro del mismo
    // bucket. Incluye estadisticas (tamanio, reservas, ocupados,
    // colisiones).
    void generarReporteReservas(const TablaHashReservas& tabla,
                                 const std::string& carpetaReportes);

    // Reporte: Matriz Dispersa de la funcion actualmente activa en el
    // gestor (Fase 2, multi-funcion). Resuelve el nombre real del
    // cliente de cada asiento ocupado siguiendo su codigo_reserva
    // hasta la Tabla Hash y de ahi hasta el Arbol B de clientes,
    // exactamente como describe el enunciado 4.1 (el nodo de la
    // matriz solo guarda el codigo_reserva, nunca datos duplicados).
    void generarReporteMatrizAsientosPorFuncion(GestorAsientosPorFuncion& gestor,
                                                 TablaHashReservas& reservas,
                                                 ArbolBClientes& clientes,
                                                 const std::string& carpetaReportes);
}

#endif // REPORTES_GRAPHVIZ_H