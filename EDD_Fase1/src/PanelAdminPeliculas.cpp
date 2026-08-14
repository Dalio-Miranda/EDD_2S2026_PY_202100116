#include "PanelAdminPeliculas.h"
#include "ReportesGraphviz.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QDate>
#include <ctime>
#include <cstdio>
#include <vector>

// ---------- Helper local para calcular dias hasta una fecha ----------
static int diasHastaFechaLocal(const std::string& fecha) {
    int anio, mes, dia;
    if (std::sscanf(fecha.c_str(), "%d-%d-%d", &anio, &mes, &dia) != 3) {
        return 9999;
    }
    std::tm objetivo = {};
    objetivo.tm_year = anio - 1900;
    objetivo.tm_mon = mes - 1;
    objetivo.tm_mday = dia;
    std::time_t tiempoObjetivo = std::mktime(&objetivo);

    std::time_t ahora = std::time(nullptr);
    std::tm* hoyTm = std::localtime(&ahora);
    std::tm hoyCopia = *hoyTm;
    hoyCopia.tm_hour = 0; hoyCopia.tm_min = 0; hoyCopia.tm_sec = 0;
    std::time_t tiempoHoy = std::mktime(&hoyCopia);

    return static_cast<int>(std::difftime(tiempoObjetivo, tiempoHoy) / (60 * 60 * 24));
}

// Recorre el arbol inorden y junta las peliculas en un vector,
// solo para poder llenar la tabla facilmente.
static void recolectarInorden(NodoArbolPelicula* nodo, std::vector<Pelicula>& resultado) {
    if (nodo == nullptr) return;
    recolectarInorden(nodo->izquierdo, resultado);
    resultado.push_back(nodo->dato);
    recolectarInorden(nodo->derecho, resultado);
}

PanelAdminPeliculas::PanelAdminPeliculas(ArbolBinarioPeliculas& carteleraRef, QWidget* parent)
    : QWidget(parent), cartelera(carteleraRef) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    QHBoxLayout* layoutBotones = new QHBoxLayout();
    QPushButton* botonAgregar = new QPushButton("Agregar Pelicula");
    QPushButton* botonEliminar = new QPushButton("Eliminar");
    QPushButton* botonCSV = new QPushButton("Cargar CSV");
    QPushButton* botonReporte = new QPushButton("Generar Reporte Graphviz");

    layoutBotones->addWidget(botonAgregar);
    layoutBotones->addWidget(botonEliminar);
    layoutBotones->addWidget(botonCSV);
    layoutBotones->addWidget(botonReporte);
    layoutBotones->addStretch();

    tabla = new QTableWidget(this);
    tabla->setColumnCount(9);
    tabla->setHorizontalHeaderLabels({"Codigo", "Titulo", "Genero", "Duracion",
                                       "Clasificacion", "Idioma", "Fecha Estreno",
                                       "Fecha Fin", "Estado"});
    tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setSelectionBehavior(QAbstractItemView::SelectRows);

    layoutPrincipal->addLayout(layoutBotones);
    layoutPrincipal->addWidget(tabla);

    connect(botonAgregar, &QPushButton::clicked, this, &PanelAdminPeliculas::onAgregarPelicula);
    connect(botonEliminar, &QPushButton::clicked, this, &PanelAdminPeliculas::onEliminarPelicula);
    connect(botonCSV, &QPushButton::clicked, this, &PanelAdminPeliculas::onCargarCSV);
    connect(botonReporte, &QPushButton::clicked, this, &PanelAdminPeliculas::onGenerarReporte);

    actualizarTabla();
}

QString PanelAdminPeliculas::calcularEstado(const Pelicula& p) const {
    int diasParaEstreno = diasHastaFechaLocal(p.fechaEstreno);
    int diasParaFin = diasHastaFechaLocal(p.fechaFin);

    if (diasParaEstreno > 0) {
        return "Proximo estreno";
    } else if (diasParaFin <= 7) {
        return "Proximo a retirar";
    } else {
        return "En cartelera";
    }
}

void PanelAdminPeliculas::actualizarTabla() {
    std::vector<Pelicula> peliculas;
    recolectarInorden(cartelera.getRaiz(), peliculas);

    tabla->setRowCount(static_cast<int>(peliculas.size()));

    for (int fila = 0; fila < static_cast<int>(peliculas.size()); fila++) {
        const Pelicula& p = peliculas[fila];
        QString estado = calcularEstado(p);

        tabla->setItem(fila, 0, new QTableWidgetItem(QString::fromStdString(p.codigo)));
        tabla->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(p.titulo)));
        tabla->setItem(fila, 2, new QTableWidgetItem(QString::fromStdString(p.genero)));
        tabla->setItem(fila, 3, new QTableWidgetItem(QString::number(p.duracion) + " min"));
        tabla->setItem(fila, 4, new QTableWidgetItem(QString::fromStdString(p.clasificacion)));
        tabla->setItem(fila, 5, new QTableWidgetItem(QString::fromStdString(p.idioma)));
        tabla->setItem(fila, 6, new QTableWidgetItem(QString::fromStdString(p.fechaEstreno)));
        tabla->setItem(fila, 7, new QTableWidgetItem(QString::fromStdString(p.fechaFin)));

        QTableWidgetItem* itemEstado = new QTableWidgetItem(estado);
        if (estado == "Proximo a retirar") {
            itemEstado->setBackground(QColor("#FFE066"));
        } else if (estado == "Proximo estreno") {
            itemEstado->setBackground(QColor("#A9D6E5"));
        } else {
            itemEstado->setBackground(QColor("#8BE28B"));
        }
        tabla->setItem(fila, 8, itemEstado);
    }
}

void PanelAdminPeliculas::onAgregarPelicula() {
    QDialog dialogo(this);
    dialogo.setWindowTitle("Agregar Pelicula");
    QFormLayout* formulario = new QFormLayout(&dialogo);

    QLineEdit* campoCodigo = new QLineEdit();
    QLineEdit* campoTitulo = new QLineEdit();
    QLineEdit* campoGenero = new QLineEdit();
    QSpinBox* campoDuracion = new QSpinBox();
    campoDuracion->setRange(1, 600);
    campoDuracion->setSuffix(" min");

    QComboBox* campoClasificacion = new QComboBox();
    campoClasificacion->addItems({"A", "AA", "B15", "C"});

    QComboBox* campoIdioma = new QComboBox();
    campoIdioma->addItems({"Espanol", "Subtitulada", "Doblada"});

    QDateEdit* campoFechaEstreno = new QDateEdit(QDate::currentDate());
    campoFechaEstreno->setCalendarPopup(true);
    campoFechaEstreno->setDisplayFormat("yyyy-MM-dd");

    QDateEdit* campoFechaFin = new QDateEdit(QDate::currentDate().addMonths(2));
    campoFechaFin->setCalendarPopup(true);
    campoFechaFin->setDisplayFormat("yyyy-MM-dd");

    formulario->addRow("Codigo:", campoCodigo);
    formulario->addRow("Titulo:", campoTitulo);
    formulario->addRow("Genero:", campoGenero);
    formulario->addRow("Duracion:", campoDuracion);
    formulario->addRow("Clasificacion:", campoClasificacion);
    formulario->addRow("Idioma:", campoIdioma);
    formulario->addRow("Fecha estreno:", campoFechaEstreno);
    formulario->addRow("Fecha fin cartelera:", campoFechaFin);

    QDialogButtonBox* botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formulario->addRow(botones);
    connect(botones, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);

    if (dialogo.exec() == QDialog::Accepted) {
        if (campoCodigo->text().trimmed().isEmpty() || campoTitulo->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, "Datos incompletos", "El codigo y el titulo son obligatorios.");
            return;
        }

        Pelicula p;
        p.codigo = campoCodigo->text().trimmed().toStdString();
        p.titulo = campoTitulo->text().trimmed().toStdString();
        p.genero = campoGenero->text().trimmed().toStdString();
        p.duracion = campoDuracion->value();
        p.clasificacion = campoClasificacion->currentText().toStdString();
        p.idioma = campoIdioma->currentText().toStdString();
        p.fechaEstreno = campoFechaEstreno->date().toString("yyyy-MM-dd").toStdString();
        p.fechaFin = campoFechaFin->date().toString("yyyy-MM-dd").toStdString();

        cartelera.insertar(p);
        actualizarTabla();
        QMessageBox::information(this, "Pelicula agregada", "La pelicula se agrego correctamente a la cartelera.");
    }
}

void PanelAdminPeliculas::onEliminarPelicula() {
    int filaSeleccionada = tabla->currentRow();
    if (filaSeleccionada < 0) {
        QMessageBox::warning(this, "Sin seleccion", "Selecciona una pelicula de la tabla para eliminar.");
        return;
    }

    QString codigo = tabla->item(filaSeleccionada, 0)->text();

    auto respuesta = QMessageBox::question(this, "Confirmar eliminacion",
        "Estas seguro que deseas eliminar la pelicula " + codigo + "?");

    if (respuesta == QMessageBox::Yes) {
        bool eliminado = cartelera.eliminar(codigo.toStdString());
        if (eliminado) {
            actualizarTabla();
            QMessageBox::information(this, "Eliminada", "La pelicula se elimino correctamente.");
        } else {
            QMessageBox::warning(this, "Error", "No se pudo eliminar la pelicula.");
        }
    }
}

void PanelAdminPeliculas::onCargarCSV() {
    QString ruta = QFileDialog::getOpenFileName(this, "Seleccionar archivo CSV", "", "Archivos CSV (*.csv)");
    if (ruta.isEmpty()) return;

    int cargadas = cartelera.cargarDesdeCSV(ruta.toStdString());
    actualizarTabla();

    QMessageBox::information(this, "Carga masiva completada",
        QString("Se cargaron %1 peliculas desde el archivo.").arg(cargadas));
}

void PanelAdminPeliculas::onGenerarReporte() {
    ReportesGraphviz::generarReporteCartelera(cartelera, "../reports");

    auto respuesta = QMessageBox::information(this, "Reporte generado",
        "El reporte se genero en la carpeta reports/. Deseas abrirlo ahora?",
        QMessageBox::Yes | QMessageBox::No);

    if (respuesta == QMessageBox::Yes) {
        QDesktopServices::openUrl(QUrl::fromLocalFile("../reports/reporte1_cartelera_bst.png"));
    }
}