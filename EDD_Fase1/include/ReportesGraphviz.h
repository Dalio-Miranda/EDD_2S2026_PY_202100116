#ifndef REPORTES_GRAPHVIZ_H
#define REPORTES_GRAPHVIZ_H

#include <string>
#include "ArbolBinarioPeliculas.h"
#include "ListaCircularSolicitudes.h"
#include "ListaCircularPromociones.h"
#include "MatrizDispersaAsientos.h"

// Genera los 4 reportes graficos que pide el enunciado, usando
// Graphviz. Cada funcion escribe un archivo .dot en carpetaReportes
// y luego invoca el comando "dot" para renderizarlo como .png.
namespace ReportesGraphviz {

    // Reporte 1: Arbol Binario de Busqueda (Cartelera de Peliculas).
    // Verde = en cartelera, Amarillo = a menos de 7 dias de retirarse.
    void generarReporteCartelera(const ArbolBinarioPeliculas& arbol,
                                  const std::string& carpetaReportes);

    // Reporte 2: Lista Circular Doblemente Enlazada (Solicitudes Pendientes).
    void generarReporteSolicitudes(const ListaCircularSolicitudes& solicitudes,
                                    const std::string& carpetaReportes);

    // Reporte 3: Lista Circular de Listas (Promociones y Beneficios).
    void generarReportePromociones(const ListaCircularPromociones& promociones,
                                    const std::string& carpetaReportes);

    // Reporte 4: Matriz Dispersa (Mapa de Asientos por Funcion).
    void generarReporteMatrizAsientos(const MatrizDispersaAsientos& matriz,
                                       const std::string& carpetaReportes);
}

#endif // REPORTES_GRAPHVIZ_H