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
#include "Blockchain.h"
#include "Bloque.h"
#include "GrafoSedes.h"

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

    // --------------------- Reportes nuevos de Fase 3 ---------------------

    // Cadena de bloques como lista enlazada (el bloque mas reciente a
    // la izquierda, con flecha hacia su bloque anterior). Cada bloque
    // muestra INDEX, NONCE, HASH y PREVIOUSHASH (abreviados) y se
    // colorea de VERDE si su integridad es valida o de ROJO si fue
    // manipulado (enunciado, "Proteccion de datos"). Escribe
    // reporte_blockchain.png.
    void generarReporteBlockchain(const Blockchain& blockchain,
                                   const std::string& carpetaReportes);

    // Arbol de Merkle completo de UN bloque: cada hoja es una
    // transaccion de su DATA y cada nodo interno el hash combinado de
    // sus hijos, hasta el ROOTMERKLE. Escribe reporte_merkle.png.
    void generarReporteMerkle(const Bloque& bloque,
                               const std::string& carpetaReportes);

    // Grafo no dirigido ponderado de sedes: un vertice por sede (con
    // codigo, nombre, direccion, salas y telefono) y una arista por
    // cada par relacionado, rotulada con su peso. Escribe
    // reporte_grafo_sedes.png (Graphviz "graph" + layout neato).
    void generarReporteGrafoSedes(const GrafoSedes& grafo,
                                   const std::string& carpetaReportes);

    // Lista de adyacencia del grafo: una fila por vertice, seguida de
    // la cadena de aristas (vecino + peso) de ese vertice como lista
    // enlazada. Escribe reporte_lista_adyacencia.png.
    void generarReporteListaAdyacencia(const GrafoSedes& grafo,
                                        const std::string& carpetaReportes);
}

#endif // REPORTES_GRAPHVIZ_H