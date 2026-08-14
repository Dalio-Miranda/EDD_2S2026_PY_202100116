#include "MainWindow.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {

    setWindowTitle("CinemaUSAC - Fase 1");
    resize(900, 650);

    pilaPantallas = new QStackedWidget(this);
    setCentralWidget(pilaPantallas);

    // Por ahora los paneles de Admin y Cliente son placeholders;
    // se van a ir reemplazando por las pantallas reales con cada
    // funcionalidad (gestion de peliculas, promociones, etc).
    pilaPantallas->addWidget(crearPantallaInicio());               // indice 0: INICIO
    pilaPantallas->addWidget(crearPlaceholder("Panel Administrador (en construccion)")); // indice 1: ADMIN
    pilaPantallas->addWidget(crearPlaceholder("Panel Cliente (en construccion)"));       // indice 2: CLIENTE

    pilaPantallas->setCurrentIndex(INICIO);
}

QWidget* MainWindow::crearPantallaInicio() {
    QWidget* pantalla = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(pantalla);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(20);

    QLabel* titulo = new QLabel("CinemaUSAC");
    QFont fuenteTitulo = titulo->font();
    fuenteTitulo.setPointSize(28);
    fuenteTitulo.setBold(true);
    titulo->setFont(fuenteTitulo);
    titulo->setAlignment(Qt::AlignCenter);

    QLabel* subtitulo = new QLabel("Sistema de gestion de cine");
    subtitulo->setAlignment(Qt::AlignCenter);

    QPushButton* botonAdmin = new QPushButton("Ingresar como Administrador");
    QPushButton* botonCliente = new QPushButton("Ingresar como Cliente");
    botonAdmin->setMinimumSize(250, 45);
    botonCliente->setMinimumSize(250, 45);

    layout->addWidget(titulo);
    layout->addWidget(subtitulo);
    layout->addSpacing(30);
    layout->addWidget(botonAdmin, 0, Qt::AlignCenter);
    layout->addWidget(botonCliente, 0, Qt::AlignCenter);

    connect(botonAdmin, &QPushButton::clicked, this, [this]() {
        pilaPantallas->setCurrentIndex(ADMIN);
    });
    connect(botonCliente, &QPushButton::clicked, this, [this]() {
        pilaPantallas->setCurrentIndex(CLIENTE);
    });

    return pantalla;
}

QWidget* MainWindow::crearPlaceholder(const QString& texto) {
    QWidget* pantalla = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(pantalla);
    layout->setAlignment(Qt::AlignCenter);

    QLabel* label = new QLabel(texto);
    label->setAlignment(Qt::AlignCenter);

    QPushButton* botonVolver = new QPushButton("Volver al inicio");
    botonVolver->setMinimumSize(200, 40);

    connect(botonVolver, &QPushButton::clicked, this, [this]() {
        pilaPantallas->setCurrentIndex(INICIO);
    });

    layout->addWidget(label);
    layout->addWidget(botonVolver, 0, Qt::AlignCenter);

    return pantalla;
}