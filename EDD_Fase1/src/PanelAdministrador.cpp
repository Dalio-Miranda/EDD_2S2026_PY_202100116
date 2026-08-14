#include "PanelAdministrador.h"
#include "PanelAdminPeliculas.h"
#include "PanelAdminPromociones.h"
#include "PanelAdminSolicitudes.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>

PanelAdministrador::PanelAdministrador(ArbolBinarioPeliculas& cartelera,
                                        ListaCircularPromociones& promociones,
                                        ListaCircularSolicitudes& solicitudes,
                                        MatrizDispersaAsientos& matrizAsientos,
                                        QWidget* parent)
    : QWidget(parent) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    QHBoxLayout* layoutSuperior = new QHBoxLayout();
    QLabel* titulo = new QLabel("Panel Administrador");
    QFont fuenteTitulo = titulo->font();
    fuenteTitulo.setPointSize(16);
    fuenteTitulo.setBold(true);
    titulo->setFont(fuenteTitulo);

    QPushButton* botonVolver = new QPushButton("Volver al inicio");
    connect(botonVolver, &QPushButton::clicked, this, &PanelAdministrador::solicitudVolver);

    layoutSuperior->addWidget(titulo);
    layoutSuperior->addStretch();
    layoutSuperior->addWidget(botonVolver);

    QTabWidget* pestanas = new QTabWidget();

    pestanas->addTab(new PanelAdminPeliculas(cartelera), "Peliculas");
    pestanas->addTab(new PanelAdminPromociones(promociones), "Promociones");
    pestanas->addTab(new PanelAdminSolicitudes(solicitudes), "Solicitudes");

    QLabel* placeholderFunciones = new QLabel("Funciones y Mapa de Asientos (proximamente)");
    placeholderFunciones->setAlignment(Qt::AlignCenter);
    pestanas->addTab(placeholderFunciones, "Funciones y Asientos");

    layoutPrincipal->addLayout(layoutSuperior);
    layoutPrincipal->addWidget(pestanas);

    // Silenciar warnings de parametros no usados todavia en este constructor.
    (void)matrizAsientos;
}