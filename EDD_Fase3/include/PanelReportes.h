#ifndef PANEL_REPORTES_H
#define PANEL_REPORTES_H

#include <QWidget>
#include <QLabel>
#include "ArbolBinarioPeliculas.h"
#include "ListaCircularSolicitudes.h"
#include "ListaCircularPromociones.h"
#include "MatrizDispersaAsientos.h"
#include "ArbolAVLFunciones.h"
#include "ArbolBClientes.h"
#include "TablaHashReservas.h"
#include "GestorAsientosPorFuncion.h"
#include "GrafoSedes.h"
#include "Blockchain.h"

// Pestana que muestra los reportes de Graphviz DIRECTAMENTE dentro
// de la interfaz (no abre un visor externo). Tiene botones de
// navegacion (Anterior/Siguiente) para pasar entre los reportes tipo
// carrusel (4 de Fase 1 + 4 de Fase 2 + 4 de Fase 3), y un boton para
// (re)generar el actual.
class PanelReportes : public QWidget {
    Q_OBJECT

public:
    PanelReportes(ArbolBinarioPeliculas& carteleraRef,
                  ListaCircularSolicitudes& solicitudesRef,
                  ListaCircularPromociones& promocionesRef,
                  MatrizDispersaAsientos& matrizRef,
                  ArbolAVLFunciones& arbolFuncionesRef,
                  ArbolBClientes& arbolClientesRef,
                  TablaHashReservas& tablaReservasRef,
                  GestorAsientosPorFuncion& gestorRef,
                  GrafoSedes& grafoRef,
                  Blockchain& blockchainRef,
                  QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onAnterior();
    void onSiguiente();
    void onGenerar();

private:
    ArbolBinarioPeliculas& cartelera;
    ListaCircularSolicitudes& solicitudes;
    ListaCircularPromociones& promociones;
    MatrizDispersaAsientos& matriz;
    ArbolAVLFunciones& arbolFunciones;
    ArbolBClientes& arbolClientes;
    TablaHashReservas& tablaReservas;
    GestorAsientosPorFuncion& gestor;
    GrafoSedes& grafo;
    Blockchain& blockchain;

    int indiceActual;
    QLabel* labelTitulo;
    QLabel* labelImagen;

    void generarActual();
    void mostrarActual();
};

#endif // PANEL_REPORTES_H
