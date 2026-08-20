#include "PanelAdministrador.h"
#include "PanelAdminPeliculas.h"
#include "PanelAdminPromociones.h"
#include "PanelAdminSolicitudes.h"
#include "PanelAdminFunciones.h"
#include "PanelReportes.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QFont>

PanelAdministrador::PanelAdministrador(ArbolBinarioPeliculas& cartelera,
                                        ListaCircularPromociones& promociones,
                                        ListaCircularSolicitudes& solicitudes,
                                        MatrizDispersaAsientos& matrizAsientos,
                                        QWidget* parent)
    : QWidget(parent) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    QHBoxLayout* layoutSuperior = new QHBoxLayout();
    QLabel* titulo = new QLabel("Panel Administrador");
    titulo->setObjectName("tituloPanel");
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
    pestanas->addTab(new PanelAdminFunciones(matrizAsientos, cartelera), "Funciones y Asientos");
    pestanas->addTab(new PanelReportes(cartelera, solicitudes, promociones, matrizAsientos), "Reportes");

    layoutPrincipal->addLayout(layoutSuperior);
    layoutPrincipal->addWidget(pestanas);
}