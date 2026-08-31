#include "PanelClientePromociones.h"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QShowEvent>
#include <ctime>

static std::string obtenerFechaHoyStrCliente() {
    std::time_t ahora = std::time(nullptr);
    std::tm* hoyTm = std::localtime(&ahora);
    char buffer[11];
    std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d",
                  hoyTm->tm_year + 1900, hoyTm->tm_mon + 1, hoyTm->tm_mday);
    return std::string(buffer);
}

PanelClientePromociones::PanelClientePromociones(ListaCircularPromociones& promocionesRef, QWidget* parent)
    : QWidget(parent), promociones(promocionesRef) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    QLabel* labelPromos = new QLabel("Promociones vigentes hoy:");
    tablaPromociones = new QTableWidget(this);
    tablaPromociones->setColumnCount(4);
    tablaPromociones->setHorizontalHeaderLabels({"Codigo", "Nombre", "Vigencia", "Dias Aplicables"});
    tablaPromociones->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablaPromociones->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaPromociones->setSelectionBehavior(QAbstractItemView::SelectRows);

    QLabel* labelBeneficios = new QLabel("Beneficios:");
    tablaBeneficios = new QTableWidget(this);
    tablaBeneficios->setColumnCount(3);
    tablaBeneficios->setHorizontalHeaderLabels({"Tipo", "Descripcion", "Valor"});
    tablaBeneficios->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tablaBeneficios->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaBeneficios->setSelectionBehavior(QAbstractItemView::SelectRows);

    layoutPrincipal->addWidget(labelPromos);
    layoutPrincipal->addWidget(tablaPromociones, 2);
    layoutPrincipal->addWidget(labelBeneficios);
    layoutPrincipal->addWidget(tablaBeneficios, 1);

    connect(tablaPromociones, &QTableWidget::itemSelectionChanged, this, &PanelClientePromociones::onSeleccionCambiada);

    actualizarTablas();
}

void PanelClientePromociones::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    actualizarTablas();
}

void PanelClientePromociones::actualizarTablas() {
    tablaPromociones->setRowCount(0);
    tablaBeneficios->setRowCount(0);

    NodoPromocion* primero = promociones.getPrimero();
    if (primero == nullptr) return;

    std::string hoy = obtenerFechaHoyStrCliente();
    int total = promociones.getTamanio();
    NodoPromocion* actual = primero;

    for (int i = 0; i < total; i++) {
        const Promocion& p = actual->dato;

        // Las fechas en formato AAAA-MM-DD se pueden comparar como
        // texto directamente porque ese formato ordena igual que
        // las fechas reales (comparacion lexicografica = cronologica).
        bool vigente = (p.fechaInicio <= hoy) && (hoy <= p.fechaFin);

        if (vigente) {
            int fila = tablaPromociones->rowCount();
            tablaPromociones->insertRow(fila);

            QString dias;
            for (size_t j = 0; j < p.diasAplicables.size(); j++) {
                dias += QString::fromStdString(p.diasAplicables[j]);
                if (j + 1 < p.diasAplicables.size()) dias += ", ";
            }

            tablaPromociones->setItem(fila, 0, new QTableWidgetItem(QString::fromStdString(p.codigo)));
            tablaPromociones->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(p.nombre)));
            tablaPromociones->setItem(fila, 2, new QTableWidgetItem(
                QString::fromStdString(p.fechaInicio) + " a " + QString::fromStdString(p.fechaFin)));
            tablaPromociones->setItem(fila, 3, new QTableWidgetItem(dias));
        }

        actual = actual->siguiente;
    }
}

void PanelClientePromociones::onSeleccionCambiada() {
    tablaBeneficios->setRowCount(0);

    int fila = tablaPromociones->currentRow();
    if (fila < 0) return;

    QString codigo = tablaPromociones->item(fila, 0)->text();
    Promocion* promo = promociones.buscar(codigo.toStdString());
    if (promo == nullptr) return;

    NodoBeneficio* actual = promo->beneficios.getCabeza();
    int filaBeneficio = 0;
    while (actual != nullptr) {
        tablaBeneficios->insertRow(filaBeneficio);
        tablaBeneficios->setItem(filaBeneficio, 0, new QTableWidgetItem(QString::fromStdString(actual->dato.tipo)));
        tablaBeneficios->setItem(filaBeneficio, 1, new QTableWidgetItem(QString::fromStdString(actual->dato.descripcion)));
        tablaBeneficios->setItem(filaBeneficio, 2, new QTableWidgetItem(QString::fromStdString(actual->dato.valor)));
        actual = actual->siguiente;
        filaBeneficio++;
    }
}