#include "PanelAdminSolicitudes.h"
#include "ReportesGraphviz.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QFont>

PanelAdminSolicitudes::PanelAdminSolicitudes(ListaCircularSolicitudes& solicitudesRef, QWidget* parent)
    : QWidget(parent), solicitudes(solicitudesRef) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    QHBoxLayout* layoutBotones = new QHBoxLayout();
    QPushButton* botonAprobar = new QPushButton("Aprobar");
    QPushButton* botonRechazar = new QPushButton("Rechazar");
    QPushButton* botonReporte = new QPushButton("Generar Reporte Graphviz");

    labelContador = new QLabel();
    QFont fuenteContador = labelContador->font();
    fuenteContador.setBold(true);
    labelContador->setFont(fuenteContador);

    layoutBotones->addWidget(botonAprobar);
    layoutBotones->addWidget(botonRechazar);
    layoutBotones->addWidget(botonReporte);
    layoutBotones->addStretch();
    layoutBotones->addWidget(labelContador);

    tabla = new QTableWidget(this);
    tabla->setColumnCount(7);
    tabla->setHorizontalHeaderLabels({"Numero", "Cliente", "Telefono", "Tipo", "Descripcion", "Fecha", "Estado"});
    tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setSelectionBehavior(QAbstractItemView::SelectRows);

    layoutPrincipal->addLayout(layoutBotones);
    layoutPrincipal->addWidget(tabla);

    connect(botonAprobar, &QPushButton::clicked, this, &PanelAdminSolicitudes::onAprobar);
    connect(botonRechazar, &QPushButton::clicked, this, &PanelAdminSolicitudes::onRechazar);
    connect(botonReporte, &QPushButton::clicked, this, &PanelAdminSolicitudes::onGenerarReporte);

    actualizarTabla();
}

int PanelAdminSolicitudes::numeroSeleccionado() const {
    int fila = tabla->currentRow();
    if (fila < 0) return -1;
    return tabla->item(fila, 0)->text().toInt();
}

void PanelAdminSolicitudes::actualizarTabla() {
    tabla->setRowCount(0);

    NodoSolicitud* primero = solicitudes.getPrimero();
    if (primero != nullptr) {
        int total = solicitudes.getTamanio();
        NodoSolicitud* actual = primero;

        tabla->setRowCount(total);
        for (int fila = 0; fila < total; fila++) {
            const Solicitud& s = actual->dato;

            tabla->setItem(fila, 0, new QTableWidgetItem(QString::number(s.numero)));
            tabla->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(s.nombreCliente)));
            tabla->setItem(fila, 2, new QTableWidgetItem(QString::fromStdString(s.telefono)));
            tabla->setItem(fila, 3, new QTableWidgetItem(QString::fromStdString(s.tipoSolicitud)));
            tabla->setItem(fila, 4, new QTableWidgetItem(QString::fromStdString(s.descripcion)));
            tabla->setItem(fila, 5, new QTableWidgetItem(QString::fromStdString(s.fechaSolicitud)));

            QTableWidgetItem* itemEstado = new QTableWidgetItem(QString::fromStdString(s.estado));
            if (s.estado == "Pendiente") {
                itemEstado->setBackground(QColor("#FFE066"));
            } else if (s.estado == "En proceso") {
                itemEstado->setBackground(QColor("#A9D6E5"));
            } else if (s.estado == "Atendida") {
                itemEstado->setBackground(QColor("#8BE28B"));
            }
            tabla->setItem(fila, 6, itemEstado);

            actual = actual->siguiente;
        }
    }

    labelContador->setText(QString("Pendientes: %1").arg(solicitudes.contarPendientes()));
}

void PanelAdminSolicitudes::onAprobar() {
    int numero = numeroSeleccionado();
    if (numero < 0) {
        QMessageBox::warning(this, "Sin seleccion", "Selecciona una solicitud de la tabla primero.");
        return;
    }

    QMessageBox cuadro(this);
    cuadro.setWindowTitle("Aprobar solicitud #" + QString::number(numero));
    cuadro.setText("Como deseas marcar esta solicitud?");
    QPushButton* botonEnProceso = cuadro.addButton("En proceso", QMessageBox::ActionRole);
    QPushButton* botonAtendida = cuadro.addButton("Atendida", QMessageBox::ActionRole);
    cuadro.addButton(QMessageBox::Cancel);

    cuadro.exec();

    QString nuevoEstado;
    if (cuadro.clickedButton() == botonEnProceso) {
        nuevoEstado = "En proceso";
    } else if (cuadro.clickedButton() == botonAtendida) {
        nuevoEstado = "Atendida";
    } else {
        return; // cancelado
    }

    solicitudes.cambiarEstado(numero, nuevoEstado.toStdString());
    actualizarTabla();
}

void PanelAdminSolicitudes::onRechazar() {
    int numero = numeroSeleccionado();
    if (numero < 0) {
        QMessageBox::warning(this, "Sin seleccion", "Selecciona una solicitud de la tabla primero.");
        return;
    }

    auto respuesta = QMessageBox::question(this, "Confirmar rechazo",
        "Estas seguro que deseas rechazar la solicitud #" + QString::number(numero) + "? Se eliminara de la lista.");

    if (respuesta == QMessageBox::Yes) {
        solicitudes.eliminar(numero);
        actualizarTabla();
    }
}

void PanelAdminSolicitudes::onGenerarReporte() {
    ReportesGraphviz::generarReporteSolicitudes(solicitudes, "../reports");

    auto respuesta = QMessageBox::information(this, "Reporte generado",
        "El reporte se genero en la carpeta reports/. Deseas abrirlo ahora?",
        QMessageBox::Yes | QMessageBox::No);

    if (respuesta == QMessageBox::Yes) {
        QDesktopServices::openUrl(QUrl::fromLocalFile("../reports/reporte2_solicitudes_pendientes.png"));
    }
}