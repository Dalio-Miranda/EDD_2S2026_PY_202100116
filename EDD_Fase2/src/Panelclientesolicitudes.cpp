#include "PanelClienteSolicitudes.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QShowEvent>

PanelClienteSolicitudes::PanelClienteSolicitudes(ListaCircularSolicitudes& solicitudesRef, QWidget* parent)
    : QWidget(parent), solicitudes(solicitudesRef) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    // ---------- Seccion: crear solicitud ----------
    QGroupBox* grupoCrear = new QGroupBox("Realizar Solicitud Especial");
    QFormLayout* formulario = new QFormLayout(grupoCrear);

    campoNombre = new QLineEdit();
    campoTelefono = new QLineEdit();
    campoTipo = new QComboBox();
    campoTipo->addItems({"cumpleanos", "aniversario", "requerimiento especial", "queja", "sugerencia"});
    campoDescripcion = new QTextEdit();
    campoDescripcion->setMaximumHeight(80);

    QPushButton* botonEnviar = new QPushButton("Enviar Solicitud");

    formulario->addRow("Nombre:", campoNombre);
    formulario->addRow("Telefono:", campoTelefono);
    formulario->addRow("Tipo de solicitud:", campoTipo);
    formulario->addRow("Descripcion:", campoDescripcion);
    formulario->addRow(botonEnviar);

    // ---------- Seccion: ver / consultar solicitudes ----------
    QGroupBox* grupoConsultar = new QGroupBox("Solicitudes");
    QVBoxLayout* layoutConsultar = new QVBoxLayout(grupoConsultar);

    QHBoxLayout* layoutBusquedaConsulta = new QHBoxLayout();
    campoTelefonoConsulta = new QLineEdit();
    campoTelefonoConsulta->setPlaceholderText("Filtrar por tu numero de telefono (opcional)");
    QPushButton* botonConsultar = new QPushButton("Buscar por Telefono");
    QPushButton* botonVerTodas = new QPushButton("Ver Todas");
    layoutBusquedaConsulta->addWidget(campoTelefonoConsulta);
    layoutBusquedaConsulta->addWidget(botonConsultar);
    layoutBusquedaConsulta->addWidget(botonVerTodas);

    tablaResultados = new QTableWidget();
    tablaResultados->setColumnCount(5);
    tablaResultados->setHorizontalHeaderLabels({"Numero", "Tipo", "Descripcion", "Fecha", "Estado"});
    tablaResultados->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablaResultados->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layoutConsultar->addLayout(layoutBusquedaConsulta);
    layoutConsultar->addWidget(tablaResultados);

    layoutPrincipal->addWidget(grupoCrear);
    layoutPrincipal->addWidget(grupoConsultar);

    connect(botonEnviar, &QPushButton::clicked, this, &PanelClienteSolicitudes::onEnviarSolicitud);
    connect(botonConsultar, &QPushButton::clicked, this, &PanelClienteSolicitudes::onConsultarEstado);
    connect(botonVerTodas, &QPushButton::clicked, this, &PanelClienteSolicitudes::onVerTodas);
}

void PanelClienteSolicitudes::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    // Al entrar a la pestana se muestran TODAS las solicitudes de
    // una vez; no hace falta buscar primero para ver algo.
    mostrarResultados(solicitudes.listarTodas());
}

void PanelClienteSolicitudes::onEnviarSolicitud() {
    QString nombre = campoNombre->text().trimmed();
    QString telefono = campoTelefono->text().trimmed();
    QString descripcion = campoDescripcion->toPlainText().trimmed();

    if (nombre.isEmpty() || telefono.isEmpty() || descripcion.isEmpty()) {
        QMessageBox::warning(this, "Datos incompletos", "Nombre, telefono y descripcion son obligatorios.");
        return;
    }

    int numero = solicitudes.registrarSolicitud(
        nombre.toStdString(),
        telefono.toStdString(),
        campoTipo->currentText().toStdString(),
        descripcion.toStdString()
    );

    QMessageBox::information(this, "Solicitud enviada",
        QString("Tu solicitud fue registrada con el numero #%1. Puedes verla abajo, en la lista de todas las solicitudes.")
            .arg(numero));

    campoNombre->clear();
    campoTelefono->clear();
    campoDescripcion->clear();

    mostrarResultados(solicitudes.listarTodas());
}

void PanelClienteSolicitudes::onConsultarEstado() {
    QString telefono = campoTelefonoConsulta->text().trimmed();
    if (telefono.isEmpty()) {
        QMessageBox::warning(this, "Falta el telefono", "Ingresa un numero de telefono para filtrar, o presiona 'Ver Todas'.");
        return;
    }

    auto resultados = solicitudes.buscarPorTelefono(telefono.toStdString());
    mostrarResultados(resultados);

    if (resultados.estaVacio()) {
        QMessageBox::information(this, "Sin resultados", "No se encontraron solicitudes con ese telefono.");
    }
}

void PanelClienteSolicitudes::onVerTodas() {
    campoTelefonoConsulta->clear();
    mostrarResultados(solicitudes.listarTodas());
}

void PanelClienteSolicitudes::mostrarResultados(const ArregloDinamico<Solicitud>& resultados) {
    tablaResultados->setRowCount(resultados.getTamanio());

    for (int i = 0; i < resultados.getTamanio(); i++) {
        const Solicitud& s = resultados[i];
        tablaResultados->setItem(i, 0, new QTableWidgetItem(QString::number(s.numero)));
        tablaResultados->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(s.tipoSolicitud)));
        tablaResultados->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(s.descripcion)));
        tablaResultados->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(s.fechaSolicitud)));

        QTableWidgetItem* itemEstado = new QTableWidgetItem(QString::fromStdString(s.estado));
        if (s.estado == "Pendiente") itemEstado->setBackground(QColor("#FFE066"));
        else if (s.estado == "En proceso") itemEstado->setBackground(QColor("#A9D6E5"));
        else if (s.estado == "Atendida") itemEstado->setBackground(QColor("#8BE28B"));
        tablaResultados->setItem(i, 4, itemEstado);
    }
}