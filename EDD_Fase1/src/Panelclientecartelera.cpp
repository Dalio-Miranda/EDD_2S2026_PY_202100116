#include "PanelClienteCartelera.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <ctime>
#include <cstdio>

static int diasHastaFechaCartCliente(const std::string& fecha) {
    int anio, mes, dia;
    if (std::sscanf(fecha.c_str(), "%d-%d-%d", &anio, &mes, &dia) != 3) return 9999;
    std::tm objetivo = {};
    objetivo.tm_year = anio - 1900; objetivo.tm_mon = mes - 1; objetivo.tm_mday = dia;
    std::time_t tObjetivo = std::mktime(&objetivo);

    std::time_t ahora = std::time(nullptr);
    std::tm* hoyTm = std::localtime(&ahora);
    std::tm hoyCopia = *hoyTm;
    hoyCopia.tm_hour = 0; hoyCopia.tm_min = 0; hoyCopia.tm_sec = 0;
    std::time_t tHoy = std::mktime(&hoyCopia);

    return static_cast<int>(std::difftime(tObjetivo, tHoy) / (60 * 60 * 24));
}

static QString estadoPeliculaCliente(const Pelicula& p) {
    int diasEstreno = diasHastaFechaCartCliente(p.fechaEstreno);
    int diasFin = diasHastaFechaCartCliente(p.fechaFin);
    if (diasEstreno > 0) return "Proximo estreno";
    if (diasFin <= 7) return "Proximo a retirar";
    return "En cartelera";
}

static void recolectarInordenCliente(NodoArbolPelicula* nodo, std::vector<Pelicula>& resultado) {
    if (nodo == nullptr) return;
    recolectarInordenCliente(nodo->izquierdo, resultado);
    resultado.push_back(nodo->dato);
    recolectarInordenCliente(nodo->derecho, resultado);
}

PanelClienteCartelera::PanelClienteCartelera(ArbolBinarioPeliculas& carteleraRef, QWidget* parent)
    : QWidget(parent), cartelera(carteleraRef) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    QHBoxLayout* layoutBusqueda = new QHBoxLayout();
    campoBusqueda = new QLineEdit();
    campoBusqueda->setPlaceholderText("Buscar por codigo (ej: P001)");
    QPushButton* botonBuscar = new QPushButton("Buscar");
    QPushButton* botonVerTodas = new QPushButton("Ver toda la cartelera");

    layoutBusqueda->addWidget(campoBusqueda);
    layoutBusqueda->addWidget(botonBuscar);
    layoutBusqueda->addWidget(botonVerTodas);

    tabla = new QTableWidget(this);
    tabla->setColumnCount(8);
    tabla->setHorizontalHeaderLabels({"Codigo", "Titulo", "Genero", "Duracion",
                                       "Clasificacion", "Idioma", "Fechas", "Estado"});
    tabla->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setSelectionBehavior(QAbstractItemView::SelectRows);

    layoutPrincipal->addLayout(layoutBusqueda);
    layoutPrincipal->addWidget(tabla);

    connect(botonBuscar, &QPushButton::clicked, this, &PanelClienteCartelera::onBuscar);
    connect(botonVerTodas, &QPushButton::clicked, this, &PanelClienteCartelera::onVerTodas);

    onVerTodas();
}

void PanelClienteCartelera::mostrarPeliculas(const std::vector<Pelicula>& peliculas) {
    tabla->setRowCount(static_cast<int>(peliculas.size()));

    for (int fila = 0; fila < static_cast<int>(peliculas.size()); fila++) {
        const Pelicula& p = peliculas[fila];
        QString estado = estadoPeliculaCliente(p);

        tabla->setItem(fila, 0, new QTableWidgetItem(QString::fromStdString(p.codigo)));
        tabla->setItem(fila, 1, new QTableWidgetItem(QString::fromStdString(p.titulo)));
        tabla->setItem(fila, 2, new QTableWidgetItem(QString::fromStdString(p.genero)));
        tabla->setItem(fila, 3, new QTableWidgetItem(QString::number(p.duracion) + " min"));
        tabla->setItem(fila, 4, new QTableWidgetItem(QString::fromStdString(p.clasificacion)));
        tabla->setItem(fila, 5, new QTableWidgetItem(QString::fromStdString(p.idioma)));
        tabla->setItem(fila, 6, new QTableWidgetItem(
            QString::fromStdString(p.fechaEstreno) + " a " + QString::fromStdString(p.fechaFin)));

        QTableWidgetItem* itemEstado = new QTableWidgetItem(estado);
        if (estado == "Proximo a retirar") itemEstado->setBackground(QColor("#FFE066"));
        else if (estado == "Proximo estreno") itemEstado->setBackground(QColor("#A9D6E5"));
        else itemEstado->setBackground(QColor("#8BE28B"));
        tabla->setItem(fila, 7, itemEstado);
    }
}

void PanelClienteCartelera::onVerTodas() {
    std::vector<Pelicula> peliculas;
    recolectarInordenCliente(cartelera.getRaiz(), peliculas);
    mostrarPeliculas(peliculas);
}

void PanelClienteCartelera::onBuscar() {
    QString codigo = campoBusqueda->text().trimmed();
    if (codigo.isEmpty()) {
        onVerTodas();
        return;
    }

    Pelicula* encontrada = cartelera.buscar(codigo.toStdString());
    if (encontrada == nullptr) {
        QMessageBox::information(this, "Sin resultados", "No se encontro ninguna pelicula con ese codigo.");
        tabla->setRowCount(0);
        return;
    }

    std::vector<Pelicula> resultado;
    resultado.push_back(*encontrada);
    mostrarPeliculas(resultado);
}