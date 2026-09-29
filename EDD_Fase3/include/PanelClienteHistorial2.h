#ifndef PANEL_CLIENTE_HISTORIAL2_H
#define PANEL_CLIENTE_HISTORIAL2_H

#include <QWidget>
#include <QTableWidget>
#include <string>
#include "ArbolBClientes.h"
#include "TablaHashReservas.h"

class PanelClienteHistorial2 : public QWidget {
    Q_OBJECT

public:
    PanelClienteHistorial2(ArbolBClientes& clientesRef,
                            TablaHashReservas& reservasRef,
                            std::string idClienteRef,
                            QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private:
    ArbolBClientes& clientes;
    TablaHashReservas& reservas;
    std::string idCliente;

    QTableWidget* tabla;

    void actualizarTabla();
};

#endif // PANEL_CLIENTE_HISTORIAL2_H
