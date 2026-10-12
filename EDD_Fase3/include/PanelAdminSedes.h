#ifndef PANEL_ADMIN_SEDES_H
#define PANEL_ADMIN_SEDES_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include "GrafoSedes.h"
#include "ArbolAVLFunciones.h"
#include "ArbolBinarioPeliculas.h"

// Pestana del administrador con las funcionalidades exclusivas del
// grafo no dirigido ponderado (enunciado Fase 3, "Funciones de
// Administrador", punto 2):
//   a. Registrar nueva sede
//   c. Ver peliculas por sede
//   d. Buscar camino mas corto entre dos sedes (BFS)
//   e. Ver sedes con mayor relacion (ranking de aristas)
// (el punto b, "crear conexion", es la creacion de una funcion con
// sede asignada y vive en la pestana de Funciones.)
class PanelAdminSedes : public QWidget {
    Q_OBJECT

public:
    PanelAdminSedes(GrafoSedes& grafoRef,
                    ArbolAVLFunciones& funcionesRef,
                    ArbolBinarioPeliculas& carteleraRef,
                    QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onRegistrarSede();
    void onVerPeliculasPorSede();
    void onBuscarCamino();
    void onGenerarRanking();

private:
    GrafoSedes& grafo;
    ArbolAVLFunciones& funciones;
    ArbolBinarioPeliculas& cartelera;

    QTableWidget* tablaSedes;

    QLineEdit* campoCodigoSedeConsulta;
    QLabel* labelSedeConsulta;
    QTableWidget* tablaPeliculasPorSede;

    QComboBox* comboOrigen;
    QComboBox* comboDestino;
    QLabel* labelResultadoCamino;

    QTableWidget* tablaRanking;

    void actualizarTablaSedes();
    void actualizarCombosSedes();
};

#endif // PANEL_ADMIN_SEDES_H
