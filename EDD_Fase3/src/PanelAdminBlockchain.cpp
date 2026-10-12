#include "PanelAdminBlockchain.h"
#include "ReportesGraphviz.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialog>
#include <QScrollArea>
#include <QPixmap>
#include <QMessageBox>
#include <QHeaderView>
#include <QShowEvent>
#include <QApplication>
#include <QFont>

PanelAdminBlockchain::PanelAdminBlockchain(Blockchain& blockchainRef, QWidget* parent)
    : QWidget(parent), blockchain(blockchainRef) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    labelEstado = new QLabel();
    QFont fuente = labelEstado->font();
    fuente.setBold(true);
    labelEstado->setFont(fuente);
    labelEstado->setObjectName("labelEstadoBlockchain");

    QHBoxLayout* filaBotones = new QHBoxLayout();
    QPushButton* botonGenerar = new QPushButton("Generar Nuevo Bloque");
    QPushButton* botonReporte = new QPushButton("Ver Reporte de la Blockchain");
    QPushButton* botonMerkle = new QPushButton("Ver Arbol de Merkle del Bloque Seleccionado");
    QPushButton* botonVentas = new QPushButton("Ver Ventas Totales");
    filaBotones->addWidget(botonGenerar);
    filaBotones->addWidget(botonReporte);
    filaBotones->addWidget(botonMerkle);
    filaBotones->addWidget(botonVentas);
    filaBotones->addStretch();

    labelResultado = new QLabel("");
    labelResultado->setWordWrap(true);
    labelResultado->setObjectName("labelResultadoBlockchain");

    tablaBloques = new QTableWidget();
    tablaBloques->setObjectName("tablaBloques");
    tablaBloques->setColumnCount(8);
    tablaBloques->setHorizontalHeaderLabels({"INDEX", "TIMESTAMP", "NONCE", "# Transacciones",
                                              "HASH", "PREVIOUSHASH", "ROOTMERKLE", "Integridad"});
    tablaBloques->horizontalHeader()->setStretchLastSection(true);
    tablaBloques->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaBloques->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaBloques->setSelectionMode(QAbstractItemView::SingleSelection);

    textoVentas = new QTextEdit();
    textoVentas->setObjectName("textoVentas");
    textoVentas->setReadOnly(true);
    textoVentas->setPlaceholderText("Presiona 'Ver Ventas Totales' para ver el resumen calculado desde la blockchain.");
    textoVentas->setMaximumHeight(170);

    layoutPrincipal->addWidget(labelEstado);
    layoutPrincipal->addLayout(filaBotones);
    layoutPrincipal->addWidget(labelResultado);
    layoutPrincipal->addWidget(tablaBloques, 1);
    layoutPrincipal->addWidget(textoVentas);

    connect(botonGenerar, &QPushButton::clicked, this, &PanelAdminBlockchain::onGenerarBloque);
    connect(botonReporte, &QPushButton::clicked, this, &PanelAdminBlockchain::onVerReporteBlockchain);
    connect(botonMerkle, &QPushButton::clicked, this, &PanelAdminBlockchain::onVerMerkle);
    connect(botonVentas, &QPushButton::clicked, this, &PanelAdminBlockchain::onVerVentas);

    actualizar();
}

void PanelAdminBlockchain::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    actualizar();
}

QString PanelAdminBlockchain::abreviar(const std::string& hash) {
    if (hash.size() <= 14) return QString::fromStdString(hash);
    return QString::fromStdString(hash.substr(0, 12)) + "...";
}

void PanelAdminBlockchain::actualizar() {
    labelEstado->setText(QString("Bloques en la cadena: %1   |   Transacciones pendientes de bloque: %2 de %3 "
                                 "(se genera un bloque solo al llegar a %3)")
        .arg(blockchain.getCantidadBloques())
        .arg(blockchain.getCantidadPendientes())
        .arg(TRANSACCIONES_POR_BLOQUE_AUTOMATICO));

    // La lista enlazada se recorre desde la cabeza: el bloque mas
    // reciente aparece primero en la tabla.
    int total = blockchain.getCantidadBloques();
    tablaBloques->setRowCount(total);

    int fila = 0;
    for (NodoBloque* n = blockchain.getCabeza(); n != nullptr; n = n->siguiente, fila++) {
        const Bloque& b = n->dato;
        bool integro = Blockchain::bloqueEsIntegro(b);

        tablaBloques->setItem(fila, 0, new QTableWidgetItem(QString::number(b.index)));
        tablaBloques->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(b.timestamp)));
        tablaBloques->setItem(fila, 2, new QTableWidgetItem(QString::number(b.nonce)));
        tablaBloques->setItem(fila, 3, new QTableWidgetItem(QString::number(b.data.getTamanio())));
        tablaBloques->setItem(fila, 4, new QTableWidgetItem(abreviar(b.hash)));
        tablaBloques->setItem(fila, 5, new QTableWidgetItem(abreviar(b.previousHash)));
        tablaBloques->setItem(fila, 6, new QTableWidgetItem(abreviar(b.rootMerkle)));

        QTableWidgetItem* estado = new QTableWidgetItem(integro ? "Integro" : "MANIPULADO");
        estado->setBackground(QColor(integro ? "#8BE28B" : "#F4A6A6"));
        tablaBloques->setItem(fila, 7, estado);
    }
}

void PanelAdminBlockchain::onGenerarBloque() {
    if (blockchain.getCantidadPendientes() == 0) {
        labelResultado->setText("No hay transacciones pendientes: ningun cliente ha comprado boletos desde el ultimo bloque.");
        return;
    }

    int pendientesAntes = blockchain.getCantidadPendientes();

    // La prueba de trabajo (buscar el NONCE) puede tardar un poco.
    QApplication::setOverrideCursor(Qt::WaitCursor);
    bool generado = blockchain.generarBloque();
    QApplication::restoreOverrideCursor();

    if (generado) {
        Bloque* ultimo = blockchain.getUltimoBloque();
        labelResultado->setText(QString("Bloque #%1 generado con %2 transaccion(es). NONCE encontrado: %3. HASH: %4")
            .arg(ultimo->index).arg(pendientesAntes).arg(ultimo->nonce)
            .arg(QString::fromStdString(ultimo->hash)));
    }
    actualizar();
}

void PanelAdminBlockchain::mostrarImagen(const QString& titulo, const QString& ruta) {
    QPixmap imagen(ruta);
    if (imagen.isNull()) {
        QMessageBox::warning(this, "No se pudo mostrar el reporte",
            "No se pudo generar o abrir la imagen del reporte.\n"
            "Verifica que Graphviz ('dot') este instalado y en el PATH.");
        return;
    }

    QDialog dialogo(this);
    dialogo.setWindowTitle(titulo);
    dialogo.resize(1000, 600);
    QVBoxLayout* layout = new QVBoxLayout(&dialogo);

    QLabel* etiqueta = new QLabel();
    etiqueta->setPixmap(imagen);
    etiqueta->setAlignment(Qt::AlignCenter);

    QScrollArea* scroll = new QScrollArea();
    scroll->setWidget(etiqueta);
    layout->addWidget(scroll);

    dialogo.exec();
}

void PanelAdminBlockchain::onVerReporteBlockchain() {
    ReportesGraphviz::generarReporteBlockchain(blockchain, "../reports");
    mostrarImagen("Blockchain completa (verde = integro, rojo = manipulado)", "../reports/reporte_blockchain.png");
}

void PanelAdminBlockchain::onVerMerkle() {
    if (!blockchain.hayBloques()) {
        labelResultado->setText("Todavia no hay bloques: genera uno primero para ver su arbol de Merkle.");
        return;
    }

    auto seleccion = tablaBloques->selectionModel()->selectedRows();
    if (seleccion.isEmpty()) {
        labelResultado->setText("Selecciona en la tabla el bloque del que quieres ver el arbol de Merkle "
                                "(puede ser cualquiera, no solo el ultimo).");
        return;
    }

    int indiceBloque = tablaBloques->item(seleccion.first().row(), 0)->text().toInt();

    const Bloque* elegido = nullptr;
    for (NodoBloque* n = blockchain.getCabeza(); n != nullptr; n = n->siguiente) {
        if (n->dato.index == indiceBloque) { elegido = &n->dato; break; }
    }
    if (elegido == nullptr) return;

    ReportesGraphviz::generarReporteMerkle(*elegido, "../reports");
    mostrarImagen(QString("Arbol de Merkle - Bloque #%1").arg(indiceBloque), "../reports/reporte_merkle.png");
}

void PanelAdminBlockchain::onVerVentas() {
    // Se recorre el DATA de TODOS los bloques de la cadena (cada bloque
    // solo trae las transacciones nuevas desde el anterior, asi que el
    // total real es la suma de todos).
    Blockchain::ResumenVentas total = blockchain.calcularVentasTotales();

    QString texto;
    texto += QString("VENTAS TOTALES REGISTRADAS EN LA BLOCKCHAIN\n");
    texto += QString("Boletos vendidos: %1\n").arg(total.totalBoletos);
    texto += QString("Monto total recaudado: Q %1\n").arg(total.montoTotal, 0, 'f', 2);

    texto += "\nDesglose por funcion:\n";
    ArregloDinamico<Blockchain::LineaDesglose> porFuncion = blockchain.desglosePorFuncion();
    for (int i = 0; i < porFuncion.getTamanio(); i++) {
        texto += QString("  %1: %2 boleto(s), Q %3\n")
            .arg(QString::fromStdString(porFuncion[i].clave))
            .arg(porFuncion[i].boletos)
            .arg(porFuncion[i].monto, 0, 'f', 2);
    }

    texto += "\nDesglose por fecha de pago:\n";
    ArregloDinamico<Blockchain::LineaDesglose> porFecha = blockchain.desglosePorFecha();
    for (int i = 0; i < porFecha.getTamanio(); i++) {
        texto += QString("  %1: %2 boleto(s), Q %3\n")
            .arg(QString::fromStdString(porFecha[i].clave))
            .arg(porFecha[i].boletos)
            .arg(porFecha[i].monto, 0, 'f', 2);
    }

    if (total.totalBoletos == 0) texto += "\n(Todavia no hay ventas registradas en bloques.)";

    textoVentas->setPlainText(texto);
}
