#include "PanelClienteHistorial2.h"
#include "PersistenciaJSON.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QHeaderView>
#include <QShowEvent>

PanelClienteHistorial2::PanelClienteHistorial2(Blockchain& blockchainRef,
                                                ArbolAVLFunciones& funcionesRef,
                                                std::string idClienteRef,
                                                QWidget* parent)
    : QWidget(parent), blockchain(blockchainRef), funciones(funcionesRef), idCliente(std::move(idClienteRef)) {

    QVBoxLayout* layout = new QVBoxLayout(this);

    layout->addWidget(new QLabel("Mi historial de compras (leido desde la blockchain)"));

    tabla = new QTableWidget();
    tabla->setObjectName("tablaCompras");
    tabla->setColumnCount(10);
    tabla->setHorizontalHeaderLabels({"Transaccion", "Reserva", "Funcion", "Fila", "Columna",
                                       "Monto", "Metodo de pago", "Fecha de pago", "Registro", "Estado"});
    tabla->horizontalHeader()->setStretchLastSection(true);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setSelectionBehavior(QAbstractItemView::SelectRows);
    layout->addWidget(tabla, 1);

    // ---- Comprobante ----
    layout->addWidget(new QLabel("Comprobante de una transaccion (clic en una fila o escribe el codigo):"));
    QHBoxLayout* fila = new QHBoxLayout();
    campoCodigoTransaccion = new QLineEdit();
    campoCodigoTransaccion->setObjectName("campoCodigoTransaccion");
    campoCodigoTransaccion->setPlaceholderText("Ej: T001");
    campoCodigoTransaccion->setMaximumWidth(160);
    QPushButton* botonComprobante = new QPushButton("Ver Comprobante");
    fila->addWidget(campoCodigoTransaccion);
    fila->addWidget(botonComprobante);
    fila->addStretch();
    layout->addLayout(fila);

    textoComprobante = new QTextEdit();
    textoComprobante->setObjectName("textoComprobante");
    textoComprobante->setReadOnly(true);
    textoComprobante->setMaximumHeight(210);
    layout->addWidget(textoComprobante);

    connect(botonComprobante, &QPushButton::clicked, this, &PanelClienteHistorial2::onVerComprobante);
    connect(campoCodigoTransaccion, &QLineEdit::returnPressed, this, &PanelClienteHistorial2::onVerComprobante);
    connect(tabla, &QTableWidget::cellClicked, this, &PanelClienteHistorial2::onFilaSeleccionada);

    actualizarTabla();
}

void PanelClienteHistorial2::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    actualizarTabla();
}

void PanelClienteHistorial2::actualizarTabla() {
    std::string hoy = PersistenciaJSON::fechaHoy();

    ArregloDinamico<Transaccion> enCadena = blockchain.historialDeCliente(idCliente);
    ArregloDinamico<Transaccion> pendientes = blockchain.getPendientes();

    ArregloDinamico<Transaccion> propiasPendientes;
    for (int i = 0; i < pendientes.getTamanio(); i++) {
        if (pendientes[i].idCliente == idCliente) propiasPendientes.agregarFinal(pendientes[i]);
    }

    int total = enCadena.getTamanio() + propiasPendientes.getTamanio();
    tabla->setRowCount(total);

    for (int i = 0; i < total; i++) {
        bool esPendiente = i >= enCadena.getTamanio();
        const Transaccion& t = esPendiente ? propiasPendientes[i - enCadena.getTamanio()] : enCadena[i];

        QString registro = "Pendiente de bloque";
        if (!esPendiente) {
            Blockchain::Comprobante comp = blockchain.buscarTransaccion(t.codigoTransaccion);
            registro = QString("Bloque #%1").arg(comp.indiceBloque);
        }

        // VALIDA / VENCIDA segun la fecha de la funcion (Fase 3 la guarda).
        QString estado = "-";
        Funcion* f = funciones.buscar(t.codigoFuncion);
        if (f != nullptr && !f->fecha.empty()) {
            estado = (f->fecha < hoy) ? "VENCIDA" : "VALIDA";
        }

        tabla->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(t.codigoTransaccion)));
        tabla->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(t.codigoReserva)));
        tabla->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(t.codigoFuncion)));
        tabla->setItem(i, 3, new QTableWidgetItem(QString::number(t.fila)));
        tabla->setItem(i, 4, new QTableWidgetItem(QString::number(t.columna)));
        tabla->setItem(i, 5, new QTableWidgetItem(QString("Q %1").arg(t.monto, 0, 'f', 2)));
        tabla->setItem(i, 6, new QTableWidgetItem(QString::fromStdString(t.metodoPago)));
        tabla->setItem(i, 7, new QTableWidgetItem(QString::fromStdString(t.fechaPago)));

        QTableWidgetItem* itemRegistro = new QTableWidgetItem(registro);
        itemRegistro->setBackground(QColor(esPendiente ? "#FFE066" : "#8BE28B"));
        tabla->setItem(i, 8, itemRegistro);
        tabla->setItem(i, 9, new QTableWidgetItem(estado));
    }
}

void PanelClienteHistorial2::onFilaSeleccionada(int fila, int) {
    QTableWidgetItem* item = tabla->item(fila, 0);
    if (item == nullptr) return;
    campoCodigoTransaccion->setText(item->text());
    onVerComprobante();
}

void PanelClienteHistorial2::onVerComprobante() {
    std::string codigo = campoCodigoTransaccion->text().trimmed().toStdString();
    if (codigo.empty()) {
        textoComprobante->setPlainText("Escribe el codigo de la transaccion (ej. T001) o haz clic en una fila de la tabla.");
        return;
    }

    // 1) Ya registrada en un bloque?
    Blockchain::Comprobante comp = blockchain.buscarTransaccion(codigo);
    if (comp.encontrado && comp.transaccion.idCliente == idCliente) {
        const Transaccion& t = comp.transaccion;
        textoComprobante->setPlainText(QString(
            "COMPROBANTE DE COMPRA\n"
            "------------------------------------------\n"
            "Transaccion:   %1\n"
            "Reserva:       %2\n"
            "Cliente:       %3\n"
            "Funcion:       %4\n"
            "Asiento:       Fila %5, Columna %6\n"
            "Monto:         Q %7\n"
            "Metodo de pago: %8\n"
            "Fecha de pago: %9\n"
            "------------------------------------------\n"
            "Registrada en el bloque (INDEX): %10\n"
            "HASH del bloque: %11\n\n"
            "Esta compra forma parte inmutable de la blockchain: si cualquier dato del bloque se alterara, su HASH dejaria de coincidir.")
            .arg(QString::fromStdString(t.codigoTransaccion))
            .arg(QString::fromStdString(t.codigoReserva))
            .arg(QString::fromStdString(t.idCliente))
            .arg(QString::fromStdString(t.codigoFuncion))
            .arg(t.fila).arg(t.columna)
            .arg(t.monto, 0, 'f', 2)
            .arg(QString::fromStdString(t.metodoPago))
            .arg(QString::fromStdString(t.fechaPago))
            .arg(comp.indiceBloque)
            .arg(QString::fromStdString(comp.hashBloque)));
        return;
    }

    // 2) Todavia pendiente de bloque?
    ArregloDinamico<Transaccion> pendientes = blockchain.getPendientes();
    for (int i = 0; i < pendientes.getTamanio(); i++) {
        if (pendientes[i].codigoTransaccion == codigo && pendientes[i].idCliente == idCliente) {
            textoComprobante->setPlainText(QString(
                "La transaccion %1 (reserva %2, funcion %3) ya fue pagada, pero TODAVIA NO esta en ningun bloque.\n\n"
                "Cuando el administrador genere el proximo bloque (o se junten %4 transacciones) quedara registrada "
                "en la blockchain y podras ver aqui su INDEX y el HASH de su bloque.")
                .arg(QString::fromStdString(codigo))
                .arg(QString::fromStdString(pendientes[i].codigoReserva))
                .arg(QString::fromStdString(pendientes[i].codigoFuncion))
                .arg(TRANSACCIONES_POR_BLOQUE_AUTOMATICO));
            return;
        }
    }

    // Solo se muestran compras del propio cliente: no se revela si el
    // codigo pertenece a otra persona.
    textoComprobante->setPlainText(QString("No se encontro la transaccion '%1' entre tus compras.")
                                       .arg(QString::fromStdString(codigo)));
}
