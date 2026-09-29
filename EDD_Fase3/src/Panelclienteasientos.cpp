#include "PanelClienteAsientos.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QScrollArea>
#include <QShowEvent>
#include <QFont>

PanelClienteAsientos::PanelClienteAsientos(MatrizDispersaAsientos& matrizRef, QWidget* parent)
    : QWidget(parent), matriz(matrizRef) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    QHBoxLayout* layoutSuperior = new QHBoxLayout();
    campoNombre = new QLineEdit();
    campoNombre->setPlaceholderText("Tu nombre");
    QPushButton* botonActualizar = new QPushButton("Actualizar mapa");

    layoutSuperior->addWidget(new QLabel("Tu nombre:"));
    layoutSuperior->addWidget(campoNombre);
    layoutSuperior->addWidget(botonActualizar);
    layoutSuperior->addStretch();

    labelInfo = new QLabel("No hay ninguna funcion activa. Contacta al administrador.");
    QFont fuenteInfo = labelInfo->font();
    fuenteInfo.setBold(true);
    labelInfo->setFont(fuenteInfo);

    QLabel* leyenda = new QLabel("Verde = libre (clic para reservar) | Rojo = ocupado");

    contenedorGrilla = new QWidget();
    layoutGrilla = new QGridLayout(contenedorGrilla);
    layoutGrilla->setSpacing(4);

    QScrollArea* scroll = new QScrollArea();
    scroll->setWidget(contenedorGrilla);
    scroll->setWidgetResizable(true);

    layoutPrincipal->addLayout(layoutSuperior);
    layoutPrincipal->addWidget(labelInfo);
    layoutPrincipal->addWidget(leyenda);
    layoutPrincipal->addWidget(scroll);

    connect(botonActualizar, &QPushButton::clicked, this, &PanelClienteAsientos::onActualizar);

    dibujarGrilla();
}

void PanelClienteAsientos::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    dibujarGrilla(); // refresca por si el admin creo/cambio la funcion mientras tanto
}

void PanelClienteAsientos::onActualizar() {
    dibujarGrilla();
}

void PanelClienteAsientos::limpiarGrilla() {
    QLayoutItem* item;
    while ((item = layoutGrilla->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

void PanelClienteAsientos::dibujarGrilla() {
    limpiarGrilla();

    if (!matriz.existeFuncion()) {
        labelInfo->setText("No hay ninguna funcion activa. Contacta al administrador.");
        return;
    }

    labelInfo->setText(QString("Funcion: %1 - %2 - %3  |  Reservados: %4  |  Libres: %5  |  Total: %6")
        .arg(QString::fromStdString(matriz.getPelicula()))
        .arg(QString::fromStdString(matriz.getHorario()))
        .arg(QString::fromStdString(matriz.getSala()))
        .arg(matriz.getTotalReservados())
        .arg(matriz.getTotalLibres())
        .arg(matriz.getTotalAsientos()));

    int filas = matriz.getNumFilas();
    int columnas = matriz.getNumColumnas();

    for (int c = 1; c <= columnas; c++) {
        QLabel* etiqueta = new QLabel(QString::number(c));
        etiqueta->setAlignment(Qt::AlignCenter);
        layoutGrilla->addWidget(etiqueta, 0, c);
    }

    for (int f = 1; f <= filas; f++) {
        QLabel* etiquetaFila = new QLabel(QString::number(f));
        etiquetaFila->setAlignment(Qt::AlignCenter);
        layoutGrilla->addWidget(etiquetaFila, f, 0);

        for (int c = 1; c <= columnas; c++) {
            bool ocupado = matriz.estaOcupado(f, c);
            QString cliente = QString::fromStdString(matriz.getClienteEnAsiento(f, c));

            QPushButton* boton = new QPushButton(ocupado ? cliente.left(3) : "");
            boton->setFixedSize(45, 35);
            boton->setToolTip(ocupado ? ("Reservado por: " + cliente) : "Libre - clic para reservar");
            boton->setStyleSheet(ocupado
                ? "background-color: #F4A6A6;"
                : "background-color: #8BE28B;");

            connect(boton, &QPushButton::clicked, this, [this, f, c]() {
                onClickAsiento(f, c);
            });

            layoutGrilla->addWidget(boton, f, c);
        }
    }
}

void PanelClienteAsientos::onClickAsiento(int fila, int columna) {
    if (matriz.estaOcupado(fila, columna)) {
        QString cliente = QString::fromStdString(matriz.getClienteEnAsiento(fila, columna));
        QMessageBox::information(this, "Asiento ocupado",
            QString("Este asiento ya fue reservado por %1.\nPor favor elige otro asiento.").arg(cliente));
        return;
    }

    QString nombre = campoNombre->text().trimmed();
    if (nombre.isEmpty()) {
        QMessageBox::warning(this, "Falta tu nombre", "Ingresa tu nombre antes de reservar un asiento.");
        return;
    }

    auto respuesta = QMessageBox::question(this, "Confirmar reserva",
        QString("Deseas reservar el asiento Fila %1, Columna %2 a nombre de %3?")
            .arg(fila).arg(columna).arg(nombre));

    if (respuesta == QMessageBox::Yes) {
        bool ok = matriz.reservarAsiento(fila, columna, nombre.toStdString());
        if (ok) {
            dibujarGrilla();
            QMessageBox::information(this, "Reserva confirmada", "Tu asiento fue reservado correctamente.");
        } else {
            QMessageBox::warning(this, "No se pudo reservar", "Ese asiento ya no estaba disponible. Intenta con otro.");
            dibujarGrilla();
        }
    }
}