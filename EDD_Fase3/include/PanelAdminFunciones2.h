#ifndef PANEL_ADMIN_FUNCIONES2_H
#define PANEL_ADMIN_FUNCIONES2_H

#include <QWidget>
#include <QTableWidget>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QGridLayout>
#include "ArbolAVLFunciones.h"
#include "ArbolBinarioPeliculas.h"
#include "GestorAsientosPorFuncion.h"
#include "TablaHashReservas.h"
#include "ArbolBClientes.h"
#include "GrafoSedes.h"

class PanelAdminFunciones2 : public QWidget {
    Q_OBJECT

public:
    PanelAdminFunciones2(ArbolAVLFunciones& funcionesRef,
                          ArbolBinarioPeliculas& carteleraRef,
                          GestorAsientosPorFuncion& gestorRef,
                          TablaHashReservas& reservasRef,
                          ArbolBClientes& clientesRef,
                          GrafoSedes& grafoRef,
                          QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onCrearFuncion();
    void onEditarFuncion();
    void onEliminarFuncion();
    void onCargarFuncionActiva();
    void onCargarJSON();
    void onCambioRecorrido();
    void onClickAsiento(int fila, int columna);
    void onBuscarFuncion();

private:
    ArbolAVLFunciones& funciones;
    ArbolBinarioPeliculas& cartelera;
    GestorAsientosPorFuncion& gestor;
    TablaHashReservas& reservas;
    ArbolBClientes& clientes;
    GrafoSedes& grafo;

    QTableWidget* tablaFunciones;
    QComboBox* comboRecorrido;
    QLineEdit* campoBusqueda;
    QLabel* labelInfoActiva;
    QWidget* contenedorGrilla;
    QGridLayout* layoutGrilla;

    void actualizarListaFunciones();
    void dibujarGrilla();
    void limpiarGrilla();
    std::string generarCodigoFuncionUnico() const;
    std::string codigoSeleccionado() const;
    void seleccionarFilaPorCodigo(const std::string& codigo);
};

#endif // PANEL_ADMIN_FUNCIONES2_H
