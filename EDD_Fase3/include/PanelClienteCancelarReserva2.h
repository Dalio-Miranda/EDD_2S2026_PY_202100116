#ifndef PANEL_CLIENTE_CANCELAR_RESERVA2_H
#define PANEL_CLIENTE_CANCELAR_RESERVA2_H

#include <QWidget>
#include <QTableWidget>
#include <string>
#include "ArbolBClientes.h"
#include "TablaHashReservas.h"
#include "ArbolAVLFunciones.h"
#include "GestorAsientosPorFuncion.h"
#include "Blockchain.h"

class PanelClienteCancelarReserva2 : public QWidget {
    Q_OBJECT

public:
    PanelClienteCancelarReserva2(ArbolBClientes& clientesRef,
                                  TablaHashReservas& reservasRef,
                                  ArbolAVLFunciones& funcionesRef,
                                  GestorAsientosPorFuncion& gestorRef,
                                  Blockchain& blockchainRef,
                                  std::string idClienteRef,
                                  QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onCancelarSeleccionada();

private:
    ArbolBClientes& clientes;
    TablaHashReservas& reservas;
    ArbolAVLFunciones& funciones;
    GestorAsientosPorFuncion& gestor;
    Blockchain& blockchain;
    std::string idCliente;

    QTableWidget* tabla;

    void actualizarTabla();
};

#endif // PANEL_CLIENTE_CANCELAR_RESERVA2_H
