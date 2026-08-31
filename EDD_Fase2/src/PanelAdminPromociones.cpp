#include "PanelAdminPromociones.h"
#include "ReportesGraphviz.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QCheckBox>
#include <QGroupBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QDate>

PanelAdminPromociones::PanelAdminPromociones(ListaCircularPromociones& promocionesRef, QWidget* parent)
    : QWidget(parent), promociones(promocionesRef) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    QHBoxLayout* layoutBotones = new QHBoxLayout();
    QPushButton* botonAgregar = new QPushButton("Agregar Promocion");
    QPushButton* botonBeneficio = new QPushButton("Anadir Beneficio");
    QPushButton* botonEliminar = new QPushButton("Eliminar Promocion");
    QPushButton* botonReporte = new QPushButton("Generar Reporte Graphviz");

    layoutBotones->addWidget(botonAgregar);
    layoutBotones->addWidget(botonBeneficio);
    layoutBotones->addWidget(botonEliminar);
    layoutBotones->addWidget(botonReporte);
    layoutBotones->addStretch();

    QLabel* labelPromos = new QLabel("Promociones:");
    tablaPromociones = new QTableWidget(this);
    tablaPromociones->setColumnCount(5);
    tablaPromociones->setHorizontalHeaderLabels({"Codigo", "Nombre", "Fecha Inicio", "Fecha Fin", "Dias Aplicables"});
    tablaPromociones->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablaPromociones->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaPromociones->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaPromociones->setSortingEnabled(true);

    QLabel* labelBeneficios = new QLabel("Beneficios de la promocion seleccionada:");
    tablaBeneficios = new QTableWidget(this);
    tablaBeneficios->setColumnCount(3);
    tablaBeneficios->setHorizontalHeaderLabels({"Tipo", "Descripcion", "Valor"});
    tablaBeneficios->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablaBeneficios->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaBeneficios->setSelectionBehavior(QAbstractItemView::SelectRows);

    layoutPrincipal->addLayout(layoutBotones);
    layoutPrincipal->addWidget(labelPromos);
    layoutPrincipal->addWidget(tablaPromociones, 2);
    layoutPrincipal->addWidget(labelBeneficios);
    layoutPrincipal->addWidget(tablaBeneficios, 1);

    connect(botonAgregar, &QPushButton::clicked, this, &PanelAdminPromociones::onAgregarPromocion);
    connect(botonBeneficio, &QPushButton::clicked, this, &PanelAdminPromociones::onAnadirBeneficio);
    connect(botonEliminar, &QPushButton::clicked, this, &PanelAdminPromociones::onEliminarPromocion);
    connect(botonReporte, &QPushButton::clicked, this, &PanelAdminPromociones::onGenerarReporte);
    connect(tablaPromociones, &QTableWidget::itemSelectionChanged, this, &PanelAdminPromociones::onSeleccionCambiada);

    actualizarTablaPromociones();
}

QString PanelAdminPromociones::codigoSeleccionado() const {
    int fila = tablaPromociones->currentRow();
    if (fila < 0) return "";
    return tablaPromociones->item(fila, 0)->text();
}

void PanelAdminPromociones::actualizarTablaPromociones() {
    tablaPromociones->setSortingEnabled(false);
    tablaPromociones->setRowCount(0);
    tablaBeneficios->setRowCount(0);

    NodoPromocion* primero = promociones.getPrimero();
    if (primero == nullptr) {
        tablaPromociones->setSortingEnabled(true);
        return;
    }

    int total = promociones.getTamanio();
    NodoPromocion* actual = primero;

    tablaPromociones->setRowCount(total);
    for (int fila = 0; fila < total; fila++) {
        const Promocion& p = actual->dato;

        QString dias;
        for (size_t i = 0; i < p.diasAplicables.size(); i++) {
            dias += QString::fromStdString(p.diasAplicables[i]);
            if (i + 1 < p.diasAplicables.size()) dias += ", ";
        }

        tablaPromociones->setItem(fila, 0, new QTableWidgetItem(QString::fromStdString(p.codigo)));
        tablaPromociones->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(p.nombre)));
        tablaPromociones->setItem(fila, 2, new QTableWidgetItem(QString::fromStdString(p.fechaInicio)));
        tablaPromociones->setItem(fila, 3, new QTableWidgetItem(QString::fromStdString(p.fechaFin)));
        tablaPromociones->setItem(fila, 4, new QTableWidgetItem(dias));

        actual = actual->siguiente;
    }

    tablaPromociones->setSortingEnabled(true);
}

void PanelAdminPromociones::actualizarTablaBeneficios(const QString& codigoPromocion) {
    tablaBeneficios->setRowCount(0);
    if (codigoPromocion.isEmpty()) return;

    Promocion* promo = promociones.buscar(codigoPromocion.toStdString());
    if (promo == nullptr) return;

    NodoBeneficio* actual = promo->beneficios.getCabeza();
    int fila = 0;
    while (actual != nullptr) {
        tablaBeneficios->insertRow(fila);
        tablaBeneficios->setItem(fila, 0, new QTableWidgetItem(QString::fromStdString(actual->dato.tipo)));
        tablaBeneficios->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(actual->dato.descripcion)));
        tablaBeneficios->setItem(fila, 2, new QTableWidgetItem(QString::fromStdString(actual->dato.valor)));
        actual = actual->siguiente;
        fila++;
    }
}

void PanelAdminPromociones::onSeleccionCambiada() {
    actualizarTablaBeneficios(codigoSeleccionado());
}

void PanelAdminPromociones::onAgregarPromocion() {
    QDialog dialogo(this);
    dialogo.setWindowTitle("Agregar Promocion");
    QVBoxLayout* layoutDialogo = new QVBoxLayout(&dialogo);

    QFormLayout* formulario = new QFormLayout();
    QLineEdit* campoCodigo = new QLineEdit();
    campoCodigo->setPlaceholderText("Promo_04");
    QLineEdit* campoNombre = new QLineEdit();
    campoNombre->setPlaceholderText("Martes de Locura");

    QDateEdit* campoFechaInicio = new QDateEdit(QDate::currentDate());
    campoFechaInicio->setCalendarPopup(true);
    campoFechaInicio->setDisplayFormat("yyyy-MM-dd");

    QDateEdit* campoFechaFin = new QDateEdit(QDate::currentDate().addMonths(3));
    campoFechaFin->setCalendarPopup(true);
    campoFechaFin->setDisplayFormat("yyyy-MM-dd");

    formulario->addRow("Codigo:", campoCodigo);
    formulario->addRow("Nombre:", campoNombre);
    formulario->addRow("Fecha inicio:", campoFechaInicio);
    formulario->addRow("Fecha fin:", campoFechaFin);
    layoutDialogo->addLayout(formulario);

    QGroupBox* grupoDias = new QGroupBox("Dias aplicables:");
    QHBoxLayout* layoutDias = new QHBoxLayout(grupoDias);
    QCheckBox* dias[7];
    QString nombresDias[7] = {"Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado", "Domingo"};
    for (int i = 0; i < 7; i++) {
        dias[i] = new QCheckBox(nombresDias[i]);
        layoutDias->addWidget(dias[i]);
    }
    layoutDialogo->addWidget(grupoDias);

    QDialogButtonBox* botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layoutDialogo->addWidget(botones);
    connect(botones, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);

    if (dialogo.exec() == QDialog::Accepted) {
        if (campoCodigo->text().trimmed().isEmpty() || campoNombre->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Datos incompletos", "El codigo y el nombre son obligatorios.");
            return;
        }

        // Validacion: la fecha de fin debe ser posterior a la de inicio.
        if (campoFechaFin->date() <= campoFechaInicio->date()) {
            QMessageBox::warning(this, "Fechas invalidas",
                "La fecha de fin de la promocion debe ser posterior a la fecha de inicio.");
            return;
        }

        Promocion p;
        p.codigo = campoCodigo->text().trimmed().toStdString();
        p.nombre = campoNombre->text().trimmed().toStdString();
        p.fechaInicio = campoFechaInicio->date().toString("yyyy-MM-dd").toStdString();
        p.fechaFin = campoFechaFin->date().toString("yyyy-MM-dd").toStdString();

        for (int i = 0; i < 7; i++) {
            if (dias[i]->isChecked()) {
                p.diasAplicables.push_back(nombresDias[i].toStdString());
            }
        }

        promociones.agregar(p);
        actualizarTablaPromociones();
        QMessageBox::information(this, "Promocion agregada", "La promocion se agrego correctamente.");
    }
}

void PanelAdminPromociones::onAnadirBeneficio() {
    QString codigo = codigoSeleccionado();
    if (codigo.isEmpty()) {
        QMessageBox::warning(this, "Sin seleccion", "Selecciona una promocion de la tabla primero.");
        return;
    }

    Promocion* promo = promociones.buscar(codigo.toStdString());
    if (promo == nullptr) {
        QMessageBox::warning(this, "Error", "No se encontro la promocion seleccionada.");
        return;
    }

    QDialog dialogo(this);
    dialogo.setWindowTitle("Anadir Beneficio a " + codigo);
    QFormLayout* formulario = new QFormLayout(&dialogo);

    QComboBox* campoTipo = new QComboBox();
    campoTipo->addItems({"descuento", "combo", "2x1"});

    QLineEdit* campoDescripcion = new QLineEdit();
    campoDescripcion->setPlaceholderText("Por la compra de 1 boleto llevate el otro gratis");

    QLineEdit* campoValor = new QLineEdit();
    campoValor->setPlaceholderText("10%  o  NA");

    formulario->addRow("Tipo:", campoTipo);
    formulario->addRow("Descripcion:", campoDescripcion);
    formulario->addRow("Valor:", campoValor);

    QDialogButtonBox* botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formulario->addRow(botones);
    connect(botones, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);

    if (dialogo.exec() == QDialog::Accepted) {
        if (campoDescripcion->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Datos incompletos", "La descripcion es obligatoria.");
            return;
        }

        Beneficio b;
        b.tipo = campoTipo->currentText().toStdString();
        b.descripcion = campoDescripcion->text().trimmed().toStdString();
        b.valor = campoValor->text().trimmed().isEmpty() ? "NA" : campoValor->text().trimmed().toStdString();

        promo->beneficios.agregar(b);
        actualizarTablaBeneficios(codigo);
        QMessageBox::information(this, "Beneficio agregado", "El beneficio se agrego correctamente.");
    }
}

void PanelAdminPromociones::onEliminarPromocion() {
    QString codigo = codigoSeleccionado();
    if (codigo.isEmpty()) {
        QMessageBox::warning(this, "Sin seleccion", "Selecciona una promocion de la tabla para eliminar.");
        return;
    }

    auto respuesta = QMessageBox::question(this, "Confirmar eliminacion",
        "Estas seguro que deseas eliminar la promocion " + codigo + "?");

    if (respuesta == QMessageBox::Yes) {
        bool eliminado = promociones.eliminar(codigo.toStdString());
        if (eliminado) {
            actualizarTablaPromociones();
            QMessageBox::information(this, "Eliminada", "La promocion se elimino correctamente.");
        } else {
            QMessageBox::warning(this, "Error", "No se pudo eliminar la promocion.");
        }
    }
}

void PanelAdminPromociones::onGenerarReporte() {
    ReportesGraphviz::generarReportePromociones(promociones, "../reports");

    auto respuesta = QMessageBox::information(this, "Reporte generado",
        "El reporte se genero en la carpeta reports/. Deseas abrirlo ahora?",
        QMessageBox::Yes | QMessageBox::No);

    if (respuesta == QMessageBox::Yes) {
        QDesktopServices::openUrl(QUrl::fromLocalFile("../reports/reporte3_promociones_beneficios.png"));
    }
}