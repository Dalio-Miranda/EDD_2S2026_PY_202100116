#include "PanelClienteReservar2.h"
#include "PersistenciaJSON.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QScrollArea>
#include <QShowEvent>
#include <QFont>
#include <sstream>
#include <iomanip>

static void recolectarPeliculasReservar(NodoArbolPelicula* nodo, ArregloDinamico<Pelicula>& resultado) {
    if (nodo == nullptr) return;
    recolectarPeliculasReservar(nodo->izquierdo, resultado);
    resultado.agregarFinal(nodo->dato);
    recolectarPeliculasReservar(nodo->derecho, resultado);
}

PanelClienteReservar2::PanelClienteReservar2(ArbolBinarioPeliculas& carteleraRef,
                                              ArbolAVLFunciones& funcionesRef,
                                              GestorAsientosPorFuncion& gestorRef,
                                              TablaHashReservas& reservasRef,
                                              ArbolBClientes& clientesRef,
                                              std::string idClienteRef,
                                              QWidget* parent)
    : QWidget(parent), cartelera(carteleraRef), funciones(funcionesRef), gestor(gestorRef),
      reservas(reservasRef), clientes(clientesRef), idCliente(std::move(idClienteRef)) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    QHBoxLayout* layoutSeleccion = new QHBoxLayout();
    layoutSeleccion->addWidget(new QLabel("Pelicula:"));
    comboPelicula = new QComboBox();
    layoutSeleccion->addWidget(comboPelicula);

    layoutSeleccion->addWidget(new QLabel("Funcion:"));
    comboFuncion = new QComboBox();
    layoutSeleccion->addWidget(comboFuncion);

    QPushButton* botonCargar = new QPushButton("Ver Disponibilidad");
    layoutSeleccion->addWidget(botonCargar);
    layoutSeleccion->addStretch();

    labelInfo = new QLabel("Selecciona una pelicula y una funcion, luego presiona 'Ver Disponibilidad'.");
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

    layoutPrincipal->addLayout(layoutSeleccion);
    layoutPrincipal->addWidget(labelInfo);
    layoutPrincipal->addWidget(leyenda);
    layoutPrincipal->addWidget(scroll, 1);

    connect(comboPelicula, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PanelClienteReservar2::onCambioPelicula);
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

    ArregloDinamico<Pelicula> peliculas;
    recolectarPeliculasReservar(cartelera.getRaiz(), peliculas);
    for (const Pelicula& p : peliculas) {
        comboPelicula->addItem(QString::fromStdString(p.codigo + " - " + p.titulo), QString::fromStdString(p.codigo));
    }

    int indicePrevio = comboPelicula->findData(seleccionPrevia);
    comboPelicula->setCurrentIndex(indicePrevio >= 0 ? indicePrevio : 0);
    comboPelicula->blockSignals(false);

    if (comboPelicula->count() > 0) {
        llenarFuncionesDe(comboPelicula->currentData().toString().toStdString());
    } else {
        comboFuncion->clear();
    }
}

void PanelClienteReservar2::onCambioPelicula() {
    llenarFuncionesDe(comboPelicula->currentData().toString().toStdString());
}

void PanelClienteReservar2::llenarFuncionesDe(const std::string& codigoPelicula) {
    comboFuncion->clear();

    ArregloDinamico<Funcion> todas = funciones.listarFunciones(TipoRecorrido::INORDEN);
    for (const Funcion& f : todas) {
        if (f.codigoPelicula == codigoPelicula) {
            QString etiqueta = QString::fromStdString(f.codigoFuncion + " - " + f.horario + " - " + f.sala);
            comboFuncion->addItem(etiqueta, QString::fromStdString(f.codigoFuncion));
        }
    }
}

void PanelClienteReservar2::onCargarFuncion() {
    if (comboFuncion->count() == 0) {
        QMessageBox::information(this, "Sin funciones", "Esa pelicula no tiene funciones programadas.");
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
        labelInfo->setText("Selecciona una pelicula y una funcion, luego presiona 'Ver Disponibilidad'.");
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

    auto respuesta = QMessageBox::question(this, "Confirmar reserva",
        QString("Deseas reservar el asiento Fila %1, Columna %2?").arg(fila).arg(columna));
    if (respuesta != QMessageBox::Yes) return;

    std::string codigoReserva = generarCodigoReservaUnico();
    std::string codigoFuncion = gestor.getCodigoFuncionActiva();
    std::string hoy = PersistenciaJSON::fechaHoy();

    Reserva r(codigoReserva, codigoFuncion, idCliente, fila, columna, hoy);
    bool okReserva = reservas.insertar(r);
    if (!okReserva) {
        QMessageBox::warning(this, "Error", "No se pudo generar la reserva (codigo duplicado). Intenta de nuevo.");
        return;
    }

    bool okAsiento = gestor.reservarAsiento(fila, columna, codigoReserva);
    if (!okAsiento) {
        reservas.eliminar(codigoReserva);
        QMessageBox::warning(this, "Error", "El asiento ya no estaba disponible.");
        dibujarGrilla();
        return;
    }

    Cliente* c = clientes.buscar(idCliente);
    if (c != nullptr) {
        c->codigosReserva.agregarFinal(codigoReserva);
    }

    dibujarGrilla();
    QMessageBox::information(this, "Reserva confirmada",
        QString("Reserva %1 creada correctamente.").arg(QString::fromStdString(codigoReserva)));
}
