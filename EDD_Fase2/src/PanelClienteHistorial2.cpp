#include "PanelClienteHistorial2.h"
#include "PersistenciaJSON.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QShowEvent>
#include <QLabel>

PanelClienteHistorial2::PanelClienteHistorial2(ArbolBClientes& clientesRef,
                                                TablaHashReservas& reservasRef,
                                                std::string idClienteRef,
                                                QWidget* parent)
    : QWidget(parent), clientes(clientesRef), reservas(reservasRef), idCliente(std::move(idClienteRef)) {

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Mis Reservas"));

    tabla = new QTableWidget();
    tabla->setColumnCount(6);
    tabla->setHorizontalHeaderLabels({"Codigo", "Funcion", "Fecha", "Fila", "Columna", "Estado"});
    tabla->horizontalHeader()->setStretchLastSection(true);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layout->addWidget(tabla);
}

void PanelClienteHistorial2::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    actualizarTabla();
}

void PanelClienteHistorial2::actualizarTabla() {
    Cliente* c = clientes.buscar(idCliente);
    if (c == nullptr) {
        tabla->setRowCount(0);
        return;
    }

    std::string hoy = PersistenciaJSON::fechaHoy();

    tabla->setRowCount(c->codigosReserva.getTamanio());
    for (int i = 0; i < c->codigosReserva.getTamanio(); i++) {
        const std::string& codigo = c->codigosReserva[i];
        Reserva* r = reservas.buscar(codigo);

        if (r == nullptr) {
            tabla->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(codigo)));
            tabla->setItem(i, 1, new QTableWidgetItem("(no encontrada)"));
            continue;
        }

        std::string estado = (r->fechaReserva < hoy) ? "VENCIDA" : "VALIDA";

        tabla->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(r->codigoReserva)));
        tabla->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(r->codigoFuncion)));
        tabla->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(r->fechaReserva)));
        tabla->setItem(i, 3, new QTableWidgetItem(QString::number(r->fila)));
        tabla->setItem(i, 4, new QTableWidgetItem(QString::number(r->columna)));
        tabla->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(estado)));
    }
}
