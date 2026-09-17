#include "PanelReportes.h"
#include "ReportesGraphviz.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QScrollArea>
#include <QPixmap>
#include <QFont>
#include <QShowEvent>

// Titulos y rutas de los reportes, en el mismo orden en que
// ReportesGraphviz los genera. Los primeros 4 son de Fase 1; los
// ultimos 4 son nuevos de Fase 2.
static const int NUM_REPORTES = 8;

static const QString TITULOS[NUM_REPORTES] = {
    "Reporte 1: Arbol Binario de Busqueda (Cartelera)",
    "Reporte 2: Lista Circular Doblemente Enlazada (Solicitudes Pendientes)",
    "Reporte 3: Lista Circular de Listas (Promociones y Beneficios)",
    "Reporte 4: Matriz Dispersa Fase 1 (Mapa de Asientos)",
    "Reporte 5: Arbol AVL de Funciones",
    "Reporte 6: Arbol B de orden 4 de Clientes",
    "Reporte 7: Tabla Hash de Reservas",
    "Reporte 8: Matriz Dispersa Fase 2 (Funcion Activa)"
};

static const QString RUTAS[NUM_REPORTES] = {
    "../reports/reporte1_cartelera_bst.png",
    "../reports/reporte2_solicitudes_pendientes.png",
    "../reports/reporte3_promociones_beneficios.png",
    "../reports/reporte4_matriz_asientos.png",
    "../reports/reporte_funciones_avl.png",
    "../reports/reporte_clientes_arbolb.png",
    "../reports/reporte_reservas_hash.png",
    "../reports/reporte_matriz_asientos_funcion.png"
};

PanelReportes::PanelReportes(ArbolBinarioPeliculas& carteleraRef,
                              ListaCircularSolicitudes& solicitudesRef,
                              ListaCircularPromociones& promocionesRef,
                              MatrizDispersaAsientos& matrizRef,
                              ArbolAVLFunciones& arbolFuncionesRef,
                              ArbolBClientes& arbolClientesRef,
                              TablaHashReservas& tablaReservasRef,
                              GestorAsientosPorFuncion& gestorRef,
                              QWidget* parent)
    : QWidget(parent),
      cartelera(carteleraRef), solicitudes(solicitudesRef),
      promociones(promocionesRef), matriz(matrizRef),
      arbolFunciones(arbolFuncionesRef), arbolClientes(arbolClientesRef),
      tablaReservas(tablaReservasRef), gestor(gestorRef),
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
    indiceActual = (indiceActual + NUM_REPORTES - 1) % NUM_REPORTES;
    mostrarActual();
}

void PanelReportes::onSiguiente() {
    indiceActual = (indiceActual + 1) % NUM_REPORTES;
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
        case 4: ReportesGraphviz::generarReporteFunciones(arbolFunciones, "../reports"); break;
        case 5: ReportesGraphviz::generarReporteClientes(arbolClientes, "../reports"); break;
        case 6: ReportesGraphviz::generarReporteReservas(tablaReservas, "../reports"); break;
        case 7: ReportesGraphviz::generarReporteMatrizAsientosPorFuncion(gestor, tablaReservas, arbolClientes, "../reports"); break;
    }
}

void PanelReportes::mostrarActual() {
    labelTitulo->setText(QString("%1  (%2/%3)").arg(TITULOS[indiceActual]).arg(indiceActual + 1).arg(NUM_REPORTES));

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
