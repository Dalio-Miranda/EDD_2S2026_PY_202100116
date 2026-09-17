#include "PanelAdminFunciones2.h"
#include "PersistenciaJSON.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialog>
#include <QFormLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QScrollArea>
#include <QFileDialog>
#include <QHeaderView>
#include <QFont>
#include <QShowEvent>
#include <sstream>
#include <iomanip>

// Recolecta las peliculas de la cartelera (recorrido inorden) en un
// ArregloDinamico propio, no std::vector.
static void recolectarPeliculasF2(NodoArbolPelicula* nodo, ArregloDinamico<Pelicula>& resultado) {
    if (nodo == nullptr) return;
    recolectarPeliculasF2(nodo->izquierdo, resultado);
    resultado.agregarFinal(nodo->dato);
    recolectarPeliculasF2(nodo->derecho, resultado);
}

PanelAdminFunciones2::PanelAdminFunciones2(ArbolAVLFunciones& funcionesRef,
                                            ArbolBinarioPeliculas& carteleraRef,
                                            GestorAsientosPorFuncion& gestorRef,
                                            TablaHashReservas& reservasRef,
                                            ArbolBClientes& clientesRef,
                                            QWidget* parent)
    : QWidget(parent), funciones(funcionesRef), cartelera(carteleraRef),
      gestor(gestorRef), reservas(reservasRef), clientes(clientesRef) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    QHBoxLayout* layoutBotones = new QHBoxLayout();
    QPushButton* botonCrear = new QPushButton("Crear Funcion");
    QPushButton* botonEditar = new QPushButton("Editar Funcion");
    QPushButton* botonEliminar = new QPushButton("Eliminar Funcion");
    QPushButton* botonCargarActiva = new QPushButton("Cargar como Activa");
    QPushButton* botonJSON = new QPushButton("Cargar JSON (Peliculas+Funciones)");
    layoutBotones->addWidget(botonCrear);
    layoutBotones->addWidget(botonEditar);
    layoutBotones->addWidget(botonEliminar);
    layoutBotones->addWidget(botonCargarActiva);
    layoutBotones->addWidget(botonJSON);
    layoutBotones->addStretch();

    QHBoxLayout* layoutRecorrido = new QHBoxLayout();
    layoutRecorrido->addWidget(new QLabel("Ordenar listado por recorrido:"));
    comboRecorrido = new QComboBox();
    comboRecorrido->addItem("Inorden (por codigo, ascendente)");
    comboRecorrido->addItem("Preorden");
    comboRecorrido->addItem("Postorden");
    layoutRecorrido->addWidget(comboRecorrido);
    layoutRecorrido->addStretch();

    tablaFunciones = new QTableWidget();
    tablaFunciones->setColumnCount(6);
    tablaFunciones->setHorizontalHeaderLabels({"Codigo", "Pelicula", "Horario", "Sala", "Filas x Columnas", "Archivo Asientos"});
    tablaFunciones->horizontalHeader()->setStretchLastSection(true);
    tablaFunciones->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaFunciones->setSelectionMode(QAbstractItemView::SingleSelection);
    tablaFunciones->setEditTriggers(QAbstractItemView::NoEditTriggers);

    labelInfoActiva = new QLabel("Ninguna funcion activa. Selecciona una y presiona 'Cargar como Activa'.");
    QFont fuenteInfo = labelInfoActiva->font();
    fuenteInfo.setBold(true);
    labelInfoActiva->setFont(fuenteInfo);

    QLabel* leyenda = new QLabel("Verde = libre | Rojo = ocupado (clic para ver detalle de la reserva y cancelarla)");

    contenedorGrilla = new QWidget();
    layoutGrilla = new QGridLayout(contenedorGrilla);
    layoutGrilla->setSpacing(4);

    QScrollArea* scroll = new QScrollArea();
    scroll->setWidget(contenedorGrilla);
    scroll->setWidgetResizable(true);
    scroll->setMinimumHeight(200);

    layoutPrincipal->addLayout(layoutBotones);
    layoutPrincipal->addLayout(layoutRecorrido);
    layoutPrincipal->addWidget(tablaFunciones, 1);
    layoutPrincipal->addWidget(labelInfoActiva);
    layoutPrincipal->addWidget(leyenda);
    layoutPrincipal->addWidget(scroll, 1);

    connect(botonCrear, &QPushButton::clicked, this, &PanelAdminFunciones2::onCrearFuncion);
    connect(botonEditar, &QPushButton::clicked, this, &PanelAdminFunciones2::onEditarFuncion);
    connect(botonEliminar, &QPushButton::clicked, this, &PanelAdminFunciones2::onEliminarFuncion);
    connect(botonCargarActiva, &QPushButton::clicked, this, &PanelAdminFunciones2::onCargarFuncionActiva);
    connect(botonJSON, &QPushButton::clicked, this, &PanelAdminFunciones2::onCargarJSON);
    connect(comboRecorrido, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PanelAdminFunciones2::onCambioRecorrido);
}

void PanelAdminFunciones2::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    actualizarListaFunciones();
    dibujarGrilla();
}

void PanelAdminFunciones2::onCambioRecorrido() {
    actualizarListaFunciones();
}

std::string PanelAdminFunciones2::codigoSeleccionado() const {
    auto seleccion = tablaFunciones->selectionModel()->selectedRows();
    if (seleccion.isEmpty()) return "";
    int fila = seleccion.first().row();
    QTableWidgetItem* item = tablaFunciones->item(fila, 0);
    return item != nullptr ? item->text().toStdString() : "";
}

void PanelAdminFunciones2::actualizarListaFunciones() {
    TipoRecorrido tipo = TipoRecorrido::INORDEN;
    switch (comboRecorrido->currentIndex()) {
        case 1: tipo = TipoRecorrido::PREORDEN; break;
        case 2: tipo = TipoRecorrido::POSTORDEN; break;
        default: tipo = TipoRecorrido::INORDEN; break;
    }

    ArregloDinamico<Funcion> listado = funciones.listarFunciones(tipo);

    tablaFunciones->setRowCount(listado.getTamanio());
    for (int i = 0; i < listado.getTamanio(); i++) {
        const Funcion& f = listado[i];

        std::string tituloPelicula = f.codigoPelicula;
        Pelicula* p = cartelera.buscar(f.codigoPelicula);
        if (p != nullptr) tituloPelicula = p->codigo + " - " + p->titulo;

        tablaFunciones->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(f.codigoFuncion)));
        tablaFunciones->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(tituloPelicula)));
        tablaFunciones->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(f.horario)));
        tablaFunciones->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(f.sala)));
        tablaFunciones->setItem(i, 4, new QTableWidgetItem(QString("%1 x %2").arg(f.filas).arg(f.columnas)));
        tablaFunciones->setItem(i, 5, new QTableWidgetItem(
            f.archivoAsientos.empty() ? "(sin archivo)" : QString::fromStdString(f.archivoAsientos)));
    }
}

std::string PanelAdminFunciones2::generarCodigoFuncionUnico() const {
    int intento = funciones.getTamanio() + 1;
    while (true) {
        std::ostringstream oss;
        oss << "F" << std::setfill('0') << std::setw(3) << intento;
        std::string candidato = oss.str();
        if (!funciones.existe(candidato)) return candidato;
        intento++;
    }
}

void PanelAdminFunciones2::onCrearFuncion() {
    ArregloDinamico<Pelicula> peliculas;
    recolectarPeliculasF2(cartelera.getRaiz(), peliculas);

    if (peliculas.estaVacio()) {
        QMessageBox::warning(this, "Sin peliculas", "Primero agrega peliculas a la cartelera.");
        return;
    }

    QDialog dialogo(this);
    dialogo.setWindowTitle("Crear Funcion (Fase 2)");
    QFormLayout* formulario = new QFormLayout(&dialogo);

    QComboBox* campoPelicula = new QComboBox();
    for (const Pelicula& p : peliculas) {
        campoPelicula->addItem(QString::fromStdString(p.codigo + " - " + p.titulo), QString::fromStdString(p.codigo));
    }

    QLineEdit* campoHorario = new QLineEdit("17:00");
    QLineEdit* campoSala = new QLineEdit("Sala 1");

    QSpinBox* campoFilas = new QSpinBox();
    campoFilas->setRange(1, 30);
    campoFilas->setValue(10);

    QSpinBox* campoColumnas = new QSpinBox();
    campoColumnas->setRange(1, 30);
    campoColumnas->setValue(15);

    formulario->addRow("Pelicula:", campoPelicula);
    formulario->addRow("Horario:", campoHorario);
    formulario->addRow("Sala:", campoSala);
    formulario->addRow("Numero de filas:", campoFilas);
    formulario->addRow("Numero de columnas:", campoColumnas);

    QDialogButtonBox* botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formulario->addRow(botones);
    connect(botones, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);

    if (dialogo.exec() == QDialog::Accepted) {
        std::string horario = campoHorario->text().trimmed().toStdString();
        std::string sala = campoSala->text().trimmed().toStdString();

        if (horario.empty() || sala.empty()) {
            QMessageBox::warning(this, "Datos incompletos", "El horario y la sala no pueden estar vacios.");
            return;
        }

        std::string codigoPelicula = campoPelicula->currentData().toString().toStdString();
        std::string codigoFuncion = generarCodigoFuncionUnico();
        std::string archivo = Funcion::nombreArchivoPara(codigoFuncion);

        Funcion f(codigoFuncion, codigoPelicula, horario, sala,
                  campoFilas->value(), campoColumnas->value(), archivo);
        funciones.insertar(f);

        gestor.cargarFuncion(codigoFuncion, funciones);
        gestor.persistirActiva();

        actualizarListaFunciones();
        dibujarGrilla();
        QMessageBox::information(this, "Funcion creada",
            QString("Funcion %1 creada correctamente.").arg(QString::fromStdString(codigoFuncion)));
    }
}

void PanelAdminFunciones2::onEditarFuncion() {
    std::string codigo = codigoSeleccionado();
    if (codigo.empty()) {
        QMessageBox::information(this, "Sin seleccion", "Selecciona una funcion de la tabla primero.");
        return;
    }

    Funcion* f = funciones.buscar(codigo);
    if (f == nullptr) return;

    QDialog dialogo(this);
    dialogo.setWindowTitle("Editar Funcion " + QString::fromStdString(codigo));
    QFormLayout* formulario = new QFormLayout(&dialogo);

    QLineEdit* campoHorario = new QLineEdit(QString::fromStdString(f->horario));
    QLineEdit* campoSala = new QLineEdit(QString::fromStdString(f->sala));

    QLabel* labelDimensiones = new QLabel(QString("%1 x %2 (no editable)").arg(f->filas).arg(f->columnas));

    formulario->addRow("Horario:", campoHorario);
    formulario->addRow("Sala:", campoSala);
    formulario->addRow("Filas x Columnas:", labelDimensiones);

    QDialogButtonBox* botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formulario->addRow(botones);
    connect(botones, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);

    if (dialogo.exec() == QDialog::Accepted) {
        std::string horario = campoHorario->text().trimmed().toStdString();
        std::string sala = campoSala->text().trimmed().toStdString();

        if (horario.empty() || sala.empty()) {
            QMessageBox::warning(this, "Datos incompletos", "El horario y la sala no pueden estar vacios.");
            return;
        }

        Funcion actualizado = *f;
        actualizado.horario = horario;
        actualizado.sala = sala;
        funciones.editar(codigo, actualizado);

        actualizarListaFunciones();
        QMessageBox::information(this, "Funcion editada", "Los cambios se guardaron correctamente.");
    }
}

void PanelAdminFunciones2::onEliminarFuncion() {
    std::string codigo = codigoSeleccionado();
    if (codigo.empty()) {
        QMessageBox::information(this, "Sin seleccion", "Selecciona una funcion de la tabla primero.");
        return;
    }

    auto respuesta = QMessageBox::question(this, "Confirmar eliminacion",
        QString("Deseas eliminar la funcion %1?\n\nSe borrara tambien su archivo de asientos.")
            .arg(QString::fromStdString(codigo)));

    if (respuesta == QMessageBox::Yes) {
        gestor.eliminarArchivoFuncion(codigo);
        funciones.eliminar(codigo);

        actualizarListaFunciones();
        dibujarGrilla();
        QMessageBox::information(this, "Funcion eliminada", "La funcion se elimino correctamente.");
    }
}

void PanelAdminFunciones2::onCargarFuncionActiva() {
    std::string codigo = codigoSeleccionado();
    if (codigo.empty()) {
        QMessageBox::information(this, "Sin seleccion", "Selecciona una funcion de la tabla primero.");
        return;
    }

    bool ok = gestor.cargarFuncion(codigo, funciones);
    if (!ok) {
        QMessageBox::warning(this, "Error", "No se pudo cargar la funcion seleccionada.");
        return;
    }

    dibujarGrilla();
}

void PanelAdminFunciones2::onCargarJSON() {
    QString ruta = QFileDialog::getOpenFileName(this, "Selecciona el archivo JSON de peliculas y funciones",
                                                 QString(), "Archivos JSON (*.json)");
    if (ruta.isEmpty()) return;

    ResultadoCarga r = PersistenciaJSON::cargarPeliculasYFunciones(ruta.toStdString(), cartelera, funciones);

    if (r.huboErrorFatal) {
        QMessageBox::critical(this, "Error al leer el archivo",
            r.errores.estaVacio() ? "No se pudo procesar el archivo." : QString::fromStdString(r.errores[0]));
        return;
    }

    QString resumen = QString("Carga completada.\n\nExitosos: %1\nRechazados: %2")
        .arg(r.exitosos).arg(r.rechazados);

    if (!r.errores.estaVacio()) {
        resumen += "\n\nDetalle:\n";
        int mostrados = 0;
        for (const auto& e : r.errores) {
            if (mostrados >= 15) {
                resumen += QString("... y %1 mas.").arg(r.errores.getTamanio() - mostrados);
                break;
            }
            resumen += "- " + QString::fromStdString(e) + "\n";
            mostrados++;
        }
    }

    QMessageBox::information(this, "Resultado de la carga masiva", resumen);
    actualizarListaFunciones();
}

void PanelAdminFunciones2::limpiarGrilla() {
    QLayoutItem* item;
    while ((item = layoutGrilla->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

void PanelAdminFunciones2::dibujarGrilla() {
    limpiarGrilla();

    if (!gestor.hayFuncionActiva()) {
        labelInfoActiva->setText("Ninguna funcion activa. Selecciona una y presiona 'Cargar como Activa'.");
        return;
    }

    MatrizDispersaAsientos& matriz = gestor.getMatrizActiva();
    std::string codigoActiva = gestor.getCodigoFuncionActiva();
    Funcion* f = funciones.buscar(codigoActiva);
    std::string tituloPelicula = (f != nullptr) ? f->codigoPelicula : "?";
    Pelicula* p = (f != nullptr) ? cartelera.buscar(f->codigoPelicula) : nullptr;
    if (p != nullptr) tituloPelicula = p->titulo;

    labelInfoActiva->setText(QString("Funcion activa: %1 (%2 - %3)  |  Reservados: %4  |  Libres: %5  |  Total: %6")
        .arg(QString::fromStdString(codigoActiva))
        .arg(QString::fromStdString(tituloPelicula))
        .arg(f != nullptr ? QString::fromStdString(f->horario) : "")
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

    for (int fi = 1; fi <= filas; fi++) {
        QLabel* etiquetaFila = new QLabel(QString::number(fi));
        etiquetaFila->setAlignment(Qt::AlignCenter);
        layoutGrilla->addWidget(etiquetaFila, fi, 0);

        for (int c = 1; c <= columnas; c++) {
            bool ocupado = matriz.estaOcupado(fi, c);
            QString codigoReserva = QString::fromStdString(matriz.getClienteEnAsiento(fi, c));

            QPushButton* boton = new QPushButton(ocupado ? codigoReserva : "");
            boton->setFixedSize(55, 35);
            boton->setToolTip(ocupado ? ("Reserva: " + codigoReserva) : "Libre");
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

void PanelAdminFunciones2::onClickAsiento(int fila, int columna) {
    MatrizDispersaAsientos& matriz = gestor.getMatrizActiva();

    if (!matriz.estaOcupado(fila, columna)) {
        QMessageBox::information(this, "Asiento libre",
            QString("Fila %1, Columna %2 esta libre.").arg(fila).arg(columna));
        return;
    }

    std::string codigoReserva = matriz.getClienteEnAsiento(fila, columna);
    Reserva* r = reservas.buscar(codigoReserva);

    QString detalle = QString("Reserva: %1\n").arg(QString::fromStdString(codigoReserva));
    if (r != nullptr) {
        detalle += QString("Cliente: %1\nFecha: %2\n")
            .arg(QString::fromStdString(r->idCliente))
            .arg(QString::fromStdString(r->fechaReserva));
    }
    detalle += "\nDeseas cancelar esta reserva?";

    auto respuesta = QMessageBox::question(this, "Asiento ocupado", detalle);
    if (respuesta != QMessageBox::Yes) return;

    gestor.cancelarAsiento(fila, columna);
    reservas.eliminar(codigoReserva);

    if (r != nullptr) {
        Cliente* c = clientes.buscar(r->idCliente);
        if (c != nullptr) {
            c->codigosReserva.eliminarValor(codigoReserva);
        }
    }

    dibujarGrilla();
}
