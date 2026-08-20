#include "PanelReportes.h"
#include "ReportesGraphviz.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QPixmap>
#include <QFont>
#include <QShowEvent>

// Titulos y rutas de los 4 reportes, en el mismo orden en que
// ReportesGraphviz los genera.
static const QString TITULOS[4] = {
    "Reporte 1: Arbol Binario de Busqueda (Cartelera)",
    "Reporte 2: Lista Circular Doblemente Enlazada (Solicitudes Pendientes)",
    "Reporte 3: Lista Circular de Listas (Promociones y Beneficios)",
    "Reporte 4: Matriz Dispersa (Mapa de Asientos)"
};

static const QString RUTAS[4] = {
    "../reports/reporte1_cartelera_bst.png",
    "../reports/reporte2_solicitudes_pendientes.png",
    "../reports/reporte3_promociones_beneficios.png",
    "../reports/reporte4_matriz_asientos.png"
};

PanelReportes::PanelReportes(ArbolBinarioPeliculas& carteleraRef,
                              ListaCircularSolicitudes& solicitudesRef,
                              ListaCircularPromociones& promocionesRef,
                              MatrizDispersaAsientos& matrizRef,
                              QWidget* parent)
    : QWidget(parent),
      cartelera(carteleraRef), solicitudes(solicitudesRef),
      promociones(promocionesRef), matriz(matrizRef),
      indiceActual(0) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    labelTitulo = new QLabel();
    QFont fuenteTitulo = labelTitulo->font();
    fuenteTitulo.setBold(true);
    fuenteTitulo.setPointSize(11);
    labelTitulo->setFont(fuenteTitulo);
    labelTitulo->setAlignment(Qt::AlignCenter);

    QPushButton* botonGenerar = new QPushButton("Generar / Actualizar este reporte");

    labelImagen = new QLabel();
    labelImagen->setAlignment(Qt::AlignCenter);
    labelImagen->setMinimumHeight(400);

    QScrollArea* scroll = new QScrollArea();
    scroll->setWidget(labelImagen);
    scroll->setWidgetResizable(true);
    scroll->setAlignment(Qt::AlignCenter);

    QHBoxLayout* layoutNavegacion = new QHBoxLayout();
    QPushButton* botonAnterior = new QPushButton("< Anterior");
    QPushButton* botonSiguiente = new QPushButton("Siguiente >");
    layoutNavegacion->addWidget(botonAnterior);
    layoutNavegacion->addStretch();
    layoutNavegacion->addWidget(botonSiguiente);

    layoutPrincipal->addWidget(labelTitulo);
    layoutPrincipal->addWidget(botonGenerar, 0, Qt::AlignCenter);
    layoutPrincipal->addWidget(scroll, 1);
    layoutPrincipal->addLayout(layoutNavegacion);

    connect(botonGenerar, &QPushButton::clicked, this, &PanelReportes::onGenerar);
    connect(botonAnterior, &QPushButton::clicked, this, &PanelReportes::onAnterior);
    connect(botonSiguiente, &QPushButton::clicked, this, &PanelReportes::onSiguiente);

    mostrarActual();
}

void PanelReportes::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    mostrarActual(); // por si el reporte se genero/cambio desde otra pestana
}

void PanelReportes::onAnterior() {
    indiceActual = (indiceActual + 3) % 4; // +3 mod 4 = -1 mod 4, evita numeros negativos
    mostrarActual();
}

void PanelReportes::onSiguiente() {
    indiceActual = (indiceActual + 1) % 4;
    mostrarActual();
}

void PanelReportes::onGenerar() {
    generarActual();
    mostrarActual();
}

void PanelReportes::generarActual() {
    switch (indiceActual) {
        case 0: ReportesGraphviz::generarReporteCartelera(cartelera, "../reports"); break;
        case 1: ReportesGraphviz::generarReporteSolicitudes(solicitudes, "../reports"); break;
        case 2: ReportesGraphviz::generarReportePromociones(promociones, "../reports"); break;
        case 3: ReportesGraphviz::generarReporteMatrizAsientos(matriz, "../reports"); break;
    }
}

void PanelReportes::mostrarActual() {
    labelTitulo->setText(QString("%1  (%2/4)").arg(TITULOS[indiceActual]).arg(indiceActual + 1));

    QPixmap imagen(RUTAS[indiceActual]);
    if (imagen.isNull()) {
        labelImagen->setPixmap(QPixmap());
        labelImagen->setText("Este reporte aun no se ha generado.\n\nPresiona \"Generar / Actualizar este reporte\" para crearlo.");
    } else {
        // Escalamos para que quepa en el ancho visible, manteniendo proporcion.
        QPixmap escalada = imagen.scaledToWidth(850, Qt::SmoothTransformation);
        labelImagen->setPixmap(escalada);
    }
}