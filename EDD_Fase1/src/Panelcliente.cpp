#include "PanelCliente.h"
#include "PanelClienteCartelera.h"
#include "PanelClienteAsientos.h"
#include "PanelClienteCancelarReserva.h"
#include "PanelClientePromociones.h"
#include "PanelClienteSolicitudes.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QLabel>
#include <QFont>

PanelCliente::PanelCliente(ArbolBinarioPeliculas& cartelera,
                            ListaCircularPromociones& promociones,
                            ListaCircularSolicitudes& solicitudes,
                            MatrizDispersaAsientos& matrizAsientos,
                            QWidget* parent)
    : QWidget(parent) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    QHBoxLayout* layoutSuperior = new QHBoxLayout();
    QLabel* titulo = new QLabel("Panel Cliente");
    QFont fuenteTitulo = titulo->font();
    fuenteTitulo.setPointSize(16);
    fuenteTitulo.setBold(true);
    titulo->setFont(fuenteTitulo);

    QPushButton* botonVolver = new QPushButton("Volver al inicio");
    connect(botonVolver, &QPushButton::clicked, this, &PanelCliente::solicitudVolver);

    layoutSuperior->addWidget(titulo);
    layoutSuperior->addStretch();
    layoutSuperior->addWidget(botonVolver);

    QTabWidget* pestanas = new QTabWidget();
    pestanas->addTab(new PanelClienteCartelera(cartelera), "Cartelera");
    pestanas->addTab(new PanelClienteAsientos(matrizAsientos), "Reservar Asientos");
    pestanas->addTab(new PanelClienteCancelarReserva(matrizAsientos), "Cancelar Reserva");
    pestanas->addTab(new PanelClientePromociones(promociones), "Promociones");
    pestanas->addTab(new PanelClienteSolicitudes(solicitudes), "Solicitudes");

    layoutPrincipal->addLayout(layoutSuperior);
    layoutPrincipal->addWidget(pestanas);
}