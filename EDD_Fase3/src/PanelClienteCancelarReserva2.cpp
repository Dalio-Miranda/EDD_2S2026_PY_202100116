#include "PanelClienteCancelarReserva2.h"
#include "PersistenciaJSON.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QMessageBox>
#include <QShowEvent>

PanelClienteCancelarReserva2::PanelClienteCancelarReserva2(ArbolBClientes& clientesRef,
                                                            TablaHashReservas& reservasRef,
                                                            ArbolAVLFunciones& funcionesRef,
                                                            GestorAsientosPorFuncion& gestorRef,
                                                            Blockchain& blockchainRef,
                                                            std::string idClienteRef,
                                                            QWidget* parent)
    : QWidget(parent), clientes(clientesRef), reservas(reservasRef), funciones(funcionesRef),
      gestor(gestorRef), blockchain(blockchainRef), idCliente(std::move(idClienteRef)) {

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(new QLabel("Cancelar una Reserva"));

    tabla = new QTableWidget();
    tabla->setColumnCount(5);
    tabla->setHorizontalHeaderLabels({"Codigo", "Funcion", "Fecha", "Fila", "Columna"});
    tabla->horizontalHeader()->setStretchLastSection(true);
    tabla->setSelectionBehavior(QAbstractItemView::SelectRows);
    tabla->setSelectionMode(QAbstractItemView::SingleSelection);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);

    QHBoxLayout* layoutBotones = new QHBoxLayout();
    QPushButton* botonCancelar = new QPushButton("Cancelar Reserva Seleccionada");
    layoutBotones->addWidget(botonCancelar);
    layoutBotones->addStretch();

    layout->addWidget(tabla);
    layout->addLayout(layoutBotones);

    connect(botonCancelar, &QPushButton::clicked, this, &PanelClienteCancelarReserva2::onCancelarSeleccionada);
}

void PanelClienteCancelarReserva2::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    actualizarTabla();
}

void PanelClienteCancelarReserva2::actualizarTabla() {
    Cliente* c = clientes.buscar(idCliente);
    if (c == nullptr) {
        tabla->setRowCount(0);
        return;
    }

    tabla->setRowCount(c->codigosReserva.getTamanio());
    for (int i = 0; i < c->codigosReserva.getTamanio(); i++) {
        const std::string& codigo = c->codigosReserva[i];
        Reserva* r = reservas.buscar(codigo);
        if (r == nullptr) continue;

        tabla->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(r->codigoReserva)));
        tabla->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(r->codigoFuncion)));
        tabla->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(r->fechaReserva)));
        tabla->setItem(i, 3, new QTableWidgetItem(QString::number(r->fila)));
        tabla->setItem(i, 4, new QTableWidgetItem(QString::number(r->columna)));
    }
}

void PanelClienteCancelarReserva2::onCancelarSeleccionada() {
    auto seleccion = tabla->selectionModel()->selectedRows();
    if (seleccion.isEmpty()) {
        QMessageBox::information(this, "Sin seleccion", "Selecciona una reserva de la tabla primero.");
        return;
    }

    int fila = seleccion.first().row();
    std::string codigo = tabla->item(fila, 0)->text().toStdString();

    Reserva* r = reservas.buscar(codigo);
    if (r == nullptr) {
        QMessageBox::warning(this, "Error", "La reserva ya no existe.");
        actualizarTabla();
        return;
    }

    auto respuesta = QMessageBox::question(this, "Confirmar cancelacion",
        QString("Deseas cancelar la reserva %1?").arg(QString::fromStdString(codigo)));
    if (respuesta != QMessageBox::Yes) return;

    std::string codigoFuncion = r->codigoFuncion;
    int filaAsiento = r->fila;
    int columnaAsiento = r->columna;

    bool ok = gestor.cargarFuncion(codigoFuncion, funciones);
    if (ok) {
        gestor.cancelarAsiento(filaAsiento, columnaAsiento);
    }

    // La compra ya esta (o esta por estar) en la blockchain y un bloque no
    // se puede modificar: la cancelacion se registra como una transaccion
    // NUEVA (mismo codigo de transaccion, estado CANCELADA).
    Transaccion pago;
    bool habiaPago = blockchain.buscarPagoDeReserva(codigo, pago);
    if (habiaPago) {
        std::string hoy = PersistenciaJSON::fechaHoy(); // AAAA-MM-DD
        Transaccion cancelacion = pago;
        cancelacion.estado = "CANCELADA";
        if (hoy.size() == 10) {
            cancelacion.fechaPago = hoy.substr(8, 2) + "-" + hoy.substr(5, 2) + "-" + hoy.substr(0, 4);
        }
        blockchain.agregarTransaccionPendiente(cancelacion);
    }

    reservas.eliminar(codigo);

    Cliente* c = clientes.buscar(idCliente);
    if (c != nullptr) {
        c->codigosReserva.eliminarValor(codigo);
    }

    actualizarTabla();
    QMessageBox::information(this, "Reserva cancelada", "La reserva se cancelo correctamente.");
}
