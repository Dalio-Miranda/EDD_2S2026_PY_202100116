#include "MainWindow.h"
#include "PanelAdministrador.h"
#include "PanelCliente.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QPixmap>
#include <QPainter>
#include <QPaintEvent>

// Widget con marca de agua: dibuja el logo de la USAC bien tenue,
// centrado y escalado, detras de todo el contenido normal de la
// pantalla de inicio (titulo, subtitulo, botones).
namespace {
class PantallaInicioWidget : public QWidget {
public:
    QPixmap fondo;

protected:
    void paintEvent(QPaintEvent* event) override {
        QWidget::paintEvent(event);
        if (fondo.isNull()) return;

        QPainter painter(this);
        painter.setOpacity(0.10); // marca de agua sutil, no debe tapar el texto

        QPixmap escalado = fondo.scaled(size() * 0.8, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        int x = (width() - escalado.width()) / 2;
        int y = (height() - escalado.height()) / 2;
        painter.drawPixmap(x, y, escalado);
    }
};
} // namespace

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {

    setWindowTitle("CinemaUSAC - Fase 2");
    resize(1000, 700);

    pilaPantallas = new QStackedWidget(this);
    setCentralWidget(pilaPantallas);

    PanelAdministrador* panelAdmin = new PanelAdministrador(cartelera, promociones, solicitudes, matrizAsientos);
    connect(panelAdmin, &PanelAdministrador::solicitudVolver, this, [this]() {
        pilaPantallas->setCurrentIndex(INICIO);
    });

    PanelCliente* panelCliente = new PanelCliente(cartelera, promociones, solicitudes, matrizAsientos);
    connect(panelCliente, &PanelCliente::solicitudVolver, this, [this]() {
        pilaPantallas->setCurrentIndex(INICIO);
    });

    pilaPantallas->addWidget(crearPantallaInicio());  // indice 0: INICIO
    pilaPantallas->addWidget(panelAdmin);             // indice 1: ADMIN
    pilaPantallas->addWidget(panelCliente);           // indice 2: CLIENTE

    pilaPantallas->setCurrentIndex(INICIO);
}

QWidget* MainWindow::crearPantallaInicio() {
    PantallaInicioWidget* pantalla = new PantallaInicioWidget();
    pantalla->fondo = QPixmap("../assets/usac_logo.png");

    QVBoxLayout* layout = new QVBoxLayout(pantalla);
    layout->setAlignment(Qt::AlignCenter);
    layout->setSpacing(14);

    QLabel* titulo = new QLabel("CinemaUSAC");
    titulo->setObjectName("tituloApp");
    QFont fuenteTitulo = titulo->font();
    fuenteTitulo.setPointSize(32);
    fuenteTitulo.setBold(true);
    titulo->setFont(fuenteTitulo);
    titulo->setAlignment(Qt::AlignCenter);

    QLabel* subtitulo = new QLabel("Sistema de gestion de cine");
    subtitulo->setAlignment(Qt::AlignCenter);

    QLabel* nombreEstudiante = new QLabel("Dalio Miranda - Carne 202100116");
    QFont fuenteNombre = nombreEstudiante->font();
    fuenteNombre.setPointSize(9);
    fuenteNombre.setItalic(true);
    nombreEstudiante->setFont(fuenteNombre);
    nombreEstudiante->setStyleSheet("color: #9aa0a6;");
    nombreEstudiante->setAlignment(Qt::AlignCenter);

    QPushButton* botonAdmin = new QPushButton("Ingresar como Administrador");
    QPushButton* botonCliente = new QPushButton("Ingresar como Cliente");
    botonAdmin->setObjectName("botonEntrada");
    botonCliente->setObjectName("botonEntrada");
    botonAdmin->setMinimumSize(280, 50);
    botonCliente->setMinimumSize(280, 50);

    layout->addWidget(titulo);
    layout->addWidget(subtitulo);
    layout->addWidget(nombreEstudiante);
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