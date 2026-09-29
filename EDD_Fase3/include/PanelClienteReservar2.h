#ifndef PANEL_CLIENTE_RESERVAR2_H
#define PANEL_CLIENTE_RESERVAR2_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <string>
#include "ArbolBinarioPeliculas.h"
#include "ArbolAVLFunciones.h"
#include "GestorAsientosPorFuncion.h"
#include "TablaHashReservas.h"
#include "ArbolBClientes.h"

class PanelClienteReservar2 : public QWidget {
    Q_OBJECT

public:
    PanelClienteReservar2(ArbolBinarioPeliculas& carteleraRef,
                           ArbolAVLFunciones& funcionesRef,
                           GestorAsientosPorFuncion& gestorRef,
                           TablaHashReservas& reservasRef,
                           ArbolBClientes& clientesRef,
                           std::string idClienteRef,
                           QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onCambioPelicula();
    void onCargarFuncion();
    void onClickAsiento(int fila, int columna);

private:
    ArbolBinarioPeliculas& cartelera;
    ArbolAVLFunciones& funciones;
    GestorAsientosPorFuncion& gestor;
    TablaHashReservas& reservas;
    ArbolBClientes& clientes;
    std::string idCliente;

    QComboBox* comboPelicula;
    QComboBox* comboFuncion;
    QLabel* labelInfo;
    QWidget* contenedorGrilla;
    QGridLayout* layoutGrilla;

    void llenarPeliculas();
    void llenarFuncionesDe(const std::string& codigoPelicula);
    void dibujarGrilla();
    void limpiarGrilla();
    std::string generarCodigoReservaUnico() const;
};

#endif // PANEL_CLIENTE_RESERVAR2_H
