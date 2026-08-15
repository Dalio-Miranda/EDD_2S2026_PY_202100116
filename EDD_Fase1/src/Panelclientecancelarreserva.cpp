#include "PanelClienteCancelarReserva.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

PanelClienteCancelarReserva::PanelClienteCancelarReserva(MatrizDispersaAsientos& matrizRef, QWidget* parent)
    : QWidget(parent), matriz(matrizRef) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    QHBoxLayout* layoutBusqueda = new QHBoxLayout();
    campoNombre = new QLineEdit();
    campoNombre->setPlaceholderText("Tu nombre (tal como lo usaste al reservar)");
    QPushButton* botonBuscar = new QPushButton("Mostrar mis reservas");

    layoutBusqueda->addWidget(campoNombre);
    layoutBusqueda->addWidget(botonBuscar);

    tabla = new QTableWidget(this);
    tabla->setColumnCount(2);
    tabla->setHorizontalHeaderLabels({"Fila", "Columna"});
    tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setSelectionBehavior(QAbstractItemView::SelectRows);

    QPushButton* botonCancelar = new QPushButton("Cancelar reserva seleccionada");

    layoutPrincipal->addLayout(layoutBusqueda);
    layoutPrincipal->addWidget(tabla);
    layoutPrincipal->addWidget(botonCancelar);

    connect(botonBuscar, &QPushButton::clicked, this, &PanelClienteCancelarReserva::onBuscarReservas);
    connect(botonCancelar, &QPushButton::clicked, this, &PanelClienteCancelarReserva::onCancelarSeleccionada);
}

void PanelClienteCancelarReserva::onBuscarReservas() {
    QString nombre = campoNombre->text().trimmed();
    if (nombre.isEmpty()) {
        QMessageBox::warning(this, "Falta tu nombre", "Ingresa tu nombre para buscar tus reservas.");
        return;
    }

    if (!matriz.existeFuncion()) {
        QMessageBox::information(this, "Sin funcion", "No hay ninguna funcion activa.");
        tabla->setRowCount(0);
        return;
    }

    auto reservas = matriz.buscarReservasDeCliente(nombre.toStdString());
    tabla->setRowCount(static_cast<int>(reservas.size()));

    for (int i = 0; i < static_cast<int>(reservas.size()); i++) {
        tabla->setItem(i, 0, new QTableWidgetItem(QString::number(reservas[i].first)));
        tabla->setItem(i, 1, new QTableWidgetItem(QString::number(reservas[i].second)));
    }

    if (reservas.empty()) {
        QMessageBox::information(this, "Sin reservas", "No se encontraron reservas activas a tu nombre.");
    }
}

void PanelClienteCancelarReserva::onCancelarSeleccionada() {
    int fila = tabla->currentRow();
    if (fila < 0) {
        QMessageBox::warning(this, "Sin seleccion", "Selecciona una reserva de la tabla primero.");
        return;
    }

    int numFila = tabla->item(fila, 0)->text().toInt();
    int numColumna = tabla->item(fila, 1)->text().toInt();

    auto respuesta = QMessageBox::question(this, "Confirmar cancelacion",
        QString("Deseas cancelar la reserva de la fila %1, columna %2?").arg(numFila).arg(numColumna));

    if (respuesta == QMessageBox::Yes) {
        bool ok = matriz.cancelarAsiento(numFila, numColumna);
        if (ok) {
            QMessageBox::information(this, "Cancelada", "Tu reserva fue cancelada correctamente.");
            onBuscarReservas(); // refresca la lista
        } else {
            QMessageBox::warning(this, "Error", "No se pudo cancelar la reserva.");
        }
    }
}