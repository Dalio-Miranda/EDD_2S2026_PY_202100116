#ifndef PANEL_ADMIN_CLIENTES_H
#define PANEL_ADMIN_CLIENTES_H

#include <QWidget>
#include <QTableWidget>
#include "ArbolBClientes.h"
#include "TablaHashReservas.h"

class PanelAdminClientes : public QWidget {
    Q_OBJECT

public:
    PanelAdminClientes(ArbolBClientes& clientesRef,
                        TablaHashReservas& reservasRef,
                        QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onAgregarCliente();
    void onEditarCliente();
    void onEliminarCliente();
    void onVerReservas();
    void onCargarJSON();

private:
    ArbolBClientes& clientes;
    TablaHashReservas& reservas;

    QTableWidget* tablaClientes;

    void actualizarListaClientes();
    std::string idSeleccionado() const;
};

#endif // PANEL_ADMIN_CLIENTES_H
