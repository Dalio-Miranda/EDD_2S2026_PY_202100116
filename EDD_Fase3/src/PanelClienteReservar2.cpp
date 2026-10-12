#include "PanelClienteReservar2.h"
#include "PersistenciaJSON.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QScrollArea>
#include <QHeaderView>
#include <QShowEvent>
#include <QApplication>
#include <QFont>
#include <sstream>
#include <iomanip>

PanelClienteReservar2::PanelClienteReservar2(ArbolBinarioPeliculas& carteleraRef,
                                              ArbolAVLFunciones& funcionesRef,
                                              GestorAsientosPorFuncion& gestorRef,
                                              TablaHashReservas& reservasRef,
                                              ArbolBClientes& clientesRef,
                                              GrafoSedes& grafoRef,
                                              Blockchain& blockchainRef,
                                              std::string idClienteRef,
                                              QWidget* parent)
    : QWidget(parent), cartelera(carteleraRef), funciones(funcionesRef), gestor(gestorRef),
      reservas(reservasRef), clientes(clientesRef), grafo(grafoRef), blockchain(blockchainRef),
      idCliente(std::move(idClienteRef)) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    // ---- Seleccion: pelicula -> sede -> funcion ----
    QHBoxLayout* layoutSeleccion = new QHBoxLayout();
    comboPelicula = new QComboBox();
    comboPelicula->setObjectName("comboPelicula");
    comboSede = new QComboBox();
    comboSede->setObjectName("comboSede");
    comboFuncion = new QComboBox();
    comboFuncion->setObjectName("comboFuncion");

    QPushButton* botonSimilares = new QPushButton("Ver Sedes Similares");
    QPushButton* botonCargar = new QPushButton("Ver Disponibilidad");

    layoutSeleccion->addWidget(new QLabel("Pelicula:"));
    layoutSeleccion->addWidget(comboPelicula);
    layoutSeleccion->addWidget(new QLabel("Sede:"));
    layoutSeleccion->addWidget(comboSede);
    layoutSeleccion->addWidget(new QLabel("Funcion:"));
    layoutSeleccion->addWidget(comboFuncion);
    layoutSeleccion->addWidget(botonSimilares);
    layoutSeleccion->addWidget(botonCargar);

    // ---- Donde esta disponible la pelicula (todas las sedes) ----
    QLabel* tituloTabla = new QLabel("Sedes donde esta disponible esta pelicula (clic en una fila para elegirla):");
    tablaFuncionesPelicula = new QTableWidget();
    tablaFuncionesPelicula->setObjectName("tablaFuncionesPelicula");
    tablaFuncionesPelicula->setColumnCount(6);
    tablaFuncionesPelicula->setHorizontalHeaderLabels({"Sede", "Nombre de la sede", "Fecha", "Horario", "Sala", "Funcion"});
    tablaFuncionesPelicula->horizontalHeader()->setStretchLastSection(true);
    tablaFuncionesPelicula->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tablaFuncionesPelicula->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaFuncionesPelicula->setMaximumHeight(150);

    labelInfo = new QLabel("Elige una pelicula y una sede, luego presiona 'Ver Disponibilidad'.");
    QFont fuenteInfo = labelInfo->font();
    fuenteInfo.setBold(true);
    labelInfo->setFont(fuenteInfo);
    labelInfo->setObjectName("labelInfoReserva");

    QLabel* leyenda = new QLabel(QString("Verde = libre (clic para reservar y pagar, Q %1 por boleto) | Rojo = ocupado")
                                     .arg(PRECIO_BOLETO, 0, 'f', 2));

    contenedorGrilla = new QWidget();
    layoutGrilla = new QGridLayout(contenedorGrilla);
    layoutGrilla->setSpacing(4);

    QScrollArea* scroll = new QScrollArea();
    scroll->setWidget(contenedorGrilla);
    scroll->setWidgetResizable(true);

    layoutPrincipal->addLayout(layoutSeleccion);
    layoutPrincipal->addWidget(tituloTabla);
    layoutPrincipal->addWidget(tablaFuncionesPelicula);
    layoutPrincipal->addWidget(labelInfo);
    layoutPrincipal->addWidget(leyenda);
    layoutPrincipal->addWidget(scroll, 1);

    connect(comboPelicula, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PanelClienteReservar2::onCambioPelicula);
    connect(comboSede, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PanelClienteReservar2::onCambioSede);
    connect(tablaFuncionesPelicula, &QTableWidget::cellClicked, this, &PanelClienteReservar2::onClickFilaFuncion);
    connect(botonSimilares, &QPushButton::clicked, this, &PanelClienteReservar2::onVerSedesSimilares);
    connect(botonCargar, &QPushButton::clicked, this, &PanelClienteReservar2::onCargarFuncion);

    llenarPeliculas();
}

void PanelClienteReservar2::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    llenarPeliculas();
    dibujarGrilla();
}

void PanelClienteReservar2::llenarPeliculas() {
    QString seleccionPrevia = comboPelicula->currentData().toString();

    comboPelicula->blockSignals(true);
    comboPelicula->clear();

    ArregloDinamico<Pelicula> peliculas = cartelera.listarPeliculas(TipoRecorrido::INORDEN);
    for (const Pelicula& p : peliculas) {
        comboPelicula->addItem(QString::fromStdString(p.codigo + " - " + p.titulo), QString::fromStdString(p.codigo));
    }

    int indicePrevio = comboPelicula->findData(seleccionPrevia);
    comboPelicula->setCurrentIndex(indicePrevio >= 0 ? indicePrevio : 0);
    comboPelicula->blockSignals(false);

    if (comboPelicula->count() > 0) {
        std::string codigo = comboPelicula->currentData().toString().toStdString();
        llenarTablaFuncionesDePelicula(codigo);
        llenarSedesDe(codigo);
    } else {
        comboSede->clear();
        comboFuncion->clear();
        tablaFuncionesPelicula->setRowCount(0);
    }
}

void PanelClienteReservar2::onCambioPelicula() {
    if (comboPelicula->count() == 0) return;
    std::string codigo = comboPelicula->currentData().toString().toStdString();
    llenarTablaFuncionesDePelicula(codigo);
    llenarSedesDe(codigo);
}

// Tabla con TODAS las funciones de la pelicula, de todas las sedes: es la
// consulta "ver sedes donde esta disponible una pelicula" del enunciado,
// con el detalle de horario y sala de cada una.
void PanelClienteReservar2::llenarTablaFuncionesDePelicula(const std::string& codigoPelicula) {
    ArregloDinamico<Funcion> todas = funciones.listarFunciones(TipoRecorrido::INORDEN);
    ArregloDinamico<Funcion> deLaPelicula;
    for (const Funcion& f : todas) {
        if (f.codigoPelicula == codigoPelicula && !f.codigoSede.empty()) deLaPelicula.agregarFinal(f);
    }

    tablaFuncionesPelicula->setRowCount(deLaPelicula.getTamanio());
    for (int i = 0; i < deLaPelicula.getTamanio(); i++) {
        const Funcion& f = deLaPelicula[i];
        Sede* sede = grafo.buscarSede(f.codigoSede);

        tablaFuncionesPelicula->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(f.codigoSede)));
        tablaFuncionesPelicula->setItem(i, 1, new QTableWidgetItem(sede ? QString::fromStdString(sede->nombre) : QString("")));
        tablaFuncionesPelicula->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(f.fecha)));
        tablaFuncionesPelicula->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(f.horario)));
        tablaFuncionesPelicula->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(f.sala)));
        tablaFuncionesPelicula->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(f.codigoFuncion)));
    }
}

void PanelClienteReservar2::llenarSedesDe(const std::string& codigoPelicula) {
    comboSede->blockSignals(true);
    comboSede->clear();

    // Sedes (vertices del grafo) donde la pelicula tiene al menos una
    // funcion, con cuantas funciones tiene en cada una.
    ArregloDinamico<Funcion> todas = funciones.listarFunciones(TipoRecorrido::INORDEN);
    ArregloDinamico<std::string> codigosSedes;
    ArregloDinamico<int> cantidades;
    for (const Funcion& f : todas) {
        if (f.codigoPelicula != codigoPelicula || f.codigoSede.empty()) continue;

        int pos = -1;
        for (int i = 0; i < codigosSedes.getTamanio(); i++) {
            if (codigosSedes[i] == f.codigoSede) { pos = i; break; }
        }
        if (pos == -1) {
            codigosSedes.agregarFinal(f.codigoSede);
            cantidades.agregarFinal(1);
        } else {
            cantidades[pos]++;
        }
    }

    for (int i = 0; i < codigosSedes.getTamanio(); i++) {
        Sede* sede = grafo.buscarSede(codigosSedes[i]);
        QString nombre = sede ? QString::fromStdString(sede->nombre) : QString("");
        comboSede->addItem(QString("%1 - %2 (%3 funcion(es))")
                               .arg(QString::fromStdString(codigosSedes[i])).arg(nombre).arg(cantidades[i]),
                           QString::fromStdString(codigosSedes[i]));
    }
    comboSede->blockSignals(false);

    if (comboSede->count() > 0) {
        llenarFuncionesDe(codigoPelicula, comboSede->currentData().toString().toStdString());
    } else {
        comboFuncion->clear();
    }
}

void PanelClienteReservar2::onCambioSede() {
    if (comboPelicula->count() == 0 || comboSede->count() == 0) return;
    llenarFuncionesDe(comboPelicula->currentData().toString().toStdString(),
                      comboSede->currentData().toString().toStdString());
}

// Filtra unicamente las funciones de esa pelicula en esa sede puntual.
void PanelClienteReservar2::llenarFuncionesDe(const std::string& codigoPelicula, const std::string& codigoSede) {
    comboFuncion->clear();

    ArregloDinamico<Funcion> todas = funciones.listarFunciones(TipoRecorrido::INORDEN);
    for (const Funcion& f : todas) {
        if (f.codigoPelicula == codigoPelicula && f.codigoSede == codigoSede) {
            comboFuncion->addItem(QString::fromStdString(f.codigoFuncion + " - " + f.fecha + " " + f.horario + " - " + f.sala),
                                  QString::fromStdString(f.codigoFuncion));
        }
    }
}

void PanelClienteReservar2::onClickFilaFuncion(int fila, int) {
    QTableWidgetItem* itemSede = tablaFuncionesPelicula->item(fila, 0);
    QTableWidgetItem* itemFuncion = tablaFuncionesPelicula->item(fila, 5);
    if (itemSede == nullptr || itemFuncion == nullptr) return;

    int idxSede = comboSede->findData(itemSede->text());
    if (idxSede >= 0) comboSede->setCurrentIndex(idxSede); // esto vuelve a llenar comboFuncion

    int idxFuncion = comboFuncion->findData(itemFuncion->text());
    if (idxFuncion >= 0) comboFuncion->setCurrentIndex(idxFuncion);
}

void PanelClienteReservar2::onVerSedesSimilares() {
    if (comboSede->count() == 0) {
        QMessageBox::information(this, "Sedes similares", "Primero elige una pelicula que tenga funciones en alguna sede.");
        return;
    }

    std::string codigoSede = comboSede->currentData().toString().toStdString();
    Sede* sede = grafo.buscarSede(codigoSede);
    ArregloDinamico<Arista> similares = grafo.sedesSimilares(codigoSede);

    QString texto;
    if (similares.estaVacio()) {
        texto = "Esta sede todavia no comparte peliculas con ninguna otra sede.";
    } else {
        texto = QString("Sedes con mayor afinidad de cartelera con %1 (mas peliculas en comun primero):\n\n")
                    .arg(sede ? QString::fromStdString(sede->nombre) : QString::fromStdString(codigoSede));
        for (int i = 0; i < similares.getTamanio(); i++) {
            Sede* otra = grafo.buscarSede(similares[i].sedeVecina);
            texto += QString("%1. %2 - %3  (%4 pelicula(s) en comun)\n")
                         .arg(i + 1)
                         .arg(QString::fromStdString(similares[i].sedeVecina))
                         .arg(otra ? QString::fromStdString(otra->nombre) : QString(""))
                         .arg(similares[i].peso);
        }
    }
    QMessageBox::information(this, "Sedes similares", texto);
}

void PanelClienteReservar2::onCargarFuncion() {
    if (comboFuncion->count() == 0) {
        QMessageBox::information(this, "Sin funciones", "No hay funciones de esa pelicula en la sede elegida.");
        return;
    }

    std::string codigoFuncion = comboFuncion->currentData().toString().toStdString();
    bool ok = gestor.cargarFuncion(codigoFuncion, funciones);
    if (!ok) {
        QMessageBox::warning(this, "Error", "No se pudo cargar la funcion seleccionada.");
        return;
    }

    dibujarGrilla();
}

void PanelClienteReservar2::limpiarGrilla() {
    QLayoutItem* item;
    while ((item = layoutGrilla->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

void PanelClienteReservar2::dibujarGrilla() {
    limpiarGrilla();

    if (!gestor.hayFuncionActiva()) {
        labelInfo->setText("Elige una pelicula y una sede, luego presiona 'Ver Disponibilidad'.");
        return;
    }

    MatrizDispersaAsientos& matriz = gestor.getMatrizActiva();
    std::string codigoActiva = gestor.getCodigoFuncionActiva();

    labelInfo->setText(QString("Funcion activa: %1  |  Libres: %2  |  Total: %3")
        .arg(QString::fromStdString(codigoActiva))
        .arg(matriz.getTotalLibres())
        .arg(matriz.getTotalAsientos()));

    int filas = matriz.getNumFilas();
    int columnas = matriz.getNumColumnas();

    for (int c = 1; c <= columnas; c++) {
        QLabel* etiqueta = new QLabel(QString::number(c));
        etiqueta->setAlignment(Qt::AlignCenter);
        layoutGrilla->addWidget(etiqueta, 0, c);
    }

    for (int fi = 1; fi <= filas; fi++) {
        QLabel* etiquetaFila = new QLabel(QString::number(fi));
        etiquetaFila->setAlignment(Qt::AlignCenter);
        layoutGrilla->addWidget(etiquetaFila, fi, 0);

        for (int c = 1; c <= columnas; c++) {
            bool ocupado = matriz.estaOcupado(fi, c);

            QPushButton* boton = new QPushButton(ocupado ? "X" : "");
            boton->setFixedSize(40, 32);
            boton->setToolTip(ocupado ? "Ocupado" : "Libre - clic para reservar");
            boton->setEnabled(!ocupado);
            boton->setStyleSheet(ocupado
                ? "background-color: #F4A6A6;"
                : "background-color: #8BE28B;");

            connect(boton, &QPushButton::clicked, this, [this, fi, c]() {
                onClickAsiento(fi, c);
            });

            layoutGrilla->addWidget(boton, fi, c);
        }
    }
}

std::string PanelClienteReservar2::generarCodigoReservaUnico() const {
    int intento = reservas.getCantidadReservas() + 1;
    while (true) {
        std::ostringstream oss;
        oss << "R" << std::setfill('0') << std::setw(3) << intento;
        std::string candidato = oss.str();
        if (!reservas.existe(candidato)) return candidato;
        intento++;
    }
}

void PanelClienteReservar2::onClickAsiento(int fila, int columna) {
    MatrizDispersaAsientos& matriz = gestor.getMatrizActiva();
    if (matriz.estaOcupado(fila, columna)) return;

    std::string codigoFuncion = gestor.getCodigoFuncionActiva();

    // ---- Confirmacion de pago (enunciado: "el cliente confirma el pago
    // indicando metodo de pago (Tarjeta o Efectivo)") ----
    QDialog dialogoPago(this);
    dialogoPago.setWindowTitle("Confirmar pago");
    QFormLayout* formulario = new QFormLayout(&dialogoPago);

    QComboBox* campoMetodo = new QComboBox();
    campoMetodo->addItems({"Tarjeta", "Efectivo"});

    formulario->addRow("Funcion:", new QLabel(QString::fromStdString(codigoFuncion)));
    formulario->addRow("Asiento:", new QLabel(QString("Fila %1, Columna %2").arg(fila).arg(columna)));
    formulario->addRow("Monto a pagar:", new QLabel(QString("Q %1").arg(PRECIO_BOLETO, 0, 'f', 2)));
    formulario->addRow("Metodo de pago:", campoMetodo);

    QDialogButtonBox* botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    botones->button(QDialogButtonBox::Ok)->setText("Confirmar pago");
    botones->button(QDialogButtonBox::Cancel)->setText("Cancelar");
    formulario->addRow(botones);
    connect(botones, &QDialogButtonBox::accepted, &dialogoPago, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialogoPago, &QDialog::reject);

    if (dialogoPago.exec() != QDialog::Accepted) return;

    std::string metodoPago = campoMetodo->currentText().toStdString();
    std::string codigoReserva = generarCodigoReservaUnico();
    std::string hoy = PersistenciaJSON::fechaHoy(); // AAAA-MM-DD

    // ---- 1) La reserva (flujo existente): Tabla Hash + matriz dispersa ----
    Reserva r(codigoReserva, codigoFuncion, idCliente, fila, columna, hoy);
    if (!reservas.insertar(r)) {
        QMessageBox::warning(this, "Error", "No se pudo generar la reserva (codigo duplicado). Intenta de nuevo.");
        return;
    }

    if (!gestor.reservarAsiento(fila, columna, codigoReserva)) {
        reservas.eliminar(codigoReserva);
        QMessageBox::warning(this, "Error", "El asiento ya no estaba disponible.");
        dibujarGrilla();
        return;
    }

    Cliente* c = clientes.buscar(idCliente);
    if (c != nullptr) c->codigosReserva.agregarFinal(codigoReserva);

    // ---- 2) La transaccion: pago de esa reserva, a la cola de pendientes
    // de la blockchain (fecha_pago en formato DD-MM-AAAA) ----
    std::string fechaPago = hoy.substr(8, 2) + "-" + hoy.substr(5, 2) + "-" + hoy.substr(0, 4);
    std::string codigoTransaccion = blockchain.generarCodigoTransaccion();

    Transaccion t(codigoTransaccion, codigoReserva, idCliente, codigoFuncion,
                  fila, columna, PRECIO_BOLETO, metodoPago, fechaPago);

    int bloquesAntes = blockchain.getCantidadBloques();
    QApplication::setOverrideCursor(Qt::WaitCursor); // la prueba de trabajo puede tardar un instante
    blockchain.agregarTransaccionPendiente(t);
    QApplication::restoreOverrideCursor();
    bool seGeneroBloque = blockchain.getCantidadBloques() > bloquesAntes;

    dibujarGrilla();

    QString mensaje = QString("Compra realizada.\n\nTransaccion: %1\nReserva: %2\nAsiento: Fila %3, Columna %4\nMonto: Q %5 (%6)\n\n")
        .arg(QString::fromStdString(codigoTransaccion))
        .arg(QString::fromStdString(codigoReserva))
        .arg(fila).arg(columna)
        .arg(PRECIO_BOLETO, 0, 'f', 2)
        .arg(QString::fromStdString(metodoPago));

    if (seGeneroBloque) {
        mensaje += QString("Tu compra ya quedo registrada en el bloque #%1 de la blockchain.")
                       .arg(blockchain.getUltimoBloque()->index);
    } else {
        mensaje += QString("Tu compra esta pendiente de ser incluida en el proximo bloque (%1 de %2 transacciones).")
                       .arg(blockchain.getCantidadPendientes()).arg(TRANSACCIONES_POR_BLOQUE_AUTOMATICO);
    }
    QMessageBox::information(this, "Compra confirmada", mensaje);
}
