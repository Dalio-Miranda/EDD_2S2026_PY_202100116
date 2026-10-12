#include "MainWindow.h"
#include "PanelAdministrador.h"
#include "PanelCliente.h"
#include "ReportesGraphviz.h"
#include "PersistenciaEDD.h"
#include <QTimer>
#include <QMessageBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>

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

    setWindowTitle("CinemaUSAC - Fase 3");
    resize(1000, 700);

    pilaPantallas = new QStackedWidget(this);
    setCentralWidget(pilaPantallas);

    // ---- Fase 3: al iniciar, se lee la blockchain de blocks/ y se
    // verifica su integridad; SOLO la parte no manipulada se carga a
    // memoria, y la Tabla Hash de reservas se reconstruye UNICAMENTE a
    // partir del DATA de esos bloques (enunciado, "Reconstruccion de la
    // Tabla Hash al iniciar el programa"). Si habia bloques en disco se
    // genera automaticamente el reporte verde/rojo.
    Blockchain::ResultadoCarga cargaBlockchain = blockchain.cargarDesdeDisco();
    blockchain.reconstruirTablaHash(tablaReservas);
    if (!blockchain.getCadenaEnDisco().estaVacio()) {
        ReportesGraphviz::generarReporteBlockchain(blockchain, "../reports");
    }
    if (cargaBlockchain.huboManipulacion) {
        QTimer::singleShot(0, this, [this, cargaBlockchain]() {
            QMessageBox::warning(this, "Blockchain manipulada",
                QString("Se detecto que los archivos de blocks/ fueron modificados.\n\n"
                        "Primer bloque manipulado: INDEX %1\n"
                        "Bloques validos cargados: %2\n"
                        "Bloques descartados: %3\n\n"
                        "Solo se cargo al sistema la parte no manipulada de la cadena. "
                        "El reporte (rojo = manipulado) esta en Administrador > Blockchain.")
                    .arg(cargaBlockchain.indiceDelPrimerManipulado)
                    .arg(cargaBlockchain.bloquesValidos)
                    .arg(cargaBlockchain.bloquesDescartados));
        });
    }

    // ---- Fase 3: persistencia LZW. Si existen sedes.edd, peliculas.edd
    // y funciones.edd (de una ejecucion anterior) se descomprimen y se
    // reconstruyen sedes, peliculas, funciones y las aristas del grafo.
    PersistenciaEDD::cargarTodo("..", grafoSedes, cartelera, arbolFunciones);

    PanelAdministrador* panelAdmin = new PanelAdministrador(cartelera, promociones, solicitudes, matrizAsientos,
                                                              arbolFunciones, arbolClientes, tablaReservas, gestorAsientos,
                                                              grafoSedes, blockchain);
    connect(panelAdmin, &PanelAdministrador::solicitudVolver, this, [this]() {
        pilaPantallas->setCurrentIndex(INICIO);
    });

    PanelCliente* panelCliente = new PanelCliente(cartelera, promociones, solicitudes, matrizAsientos,
                                                    arbolFunciones, arbolClientes, tablaReservas, gestorAsientos,
                                                    grafoSedes, blockchain);
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

    // Login del administrador: el enunciado de Fase 3 define credenciales fijas.
    connect(botonAdmin, &QPushButton::clicked, this, [this]() {
        const QString CORREO_ADMIN = "AdminCine@gmail.com";
        const QString CLAVE_ADMIN = "admin123Pass";

        QDialog dialogo(this);
        dialogo.setWindowTitle("Login de Administrador");
        QFormLayout* form = new QFormLayout(&dialogo);
        QLineEdit* campoCorreo = new QLineEdit();
        QLineEdit* campoClave = new QLineEdit();
        campoClave->setEchoMode(QLineEdit::Password);
        campoCorreo->setObjectName("campoCorreoAdmin");
        campoClave->setObjectName("campoClaveAdmin");
        form->addRow("Correo:", campoCorreo);
        form->addRow("Contrasena:", campoClave);
        QDialogButtonBox* botones =
            new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
        form->addRow(botones);
        connect(botones, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
        connect(botones, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);

        if (dialogo.exec() != QDialog::Accepted) return;

        if (campoCorreo->text().trimmed() == CORREO_ADMIN &&
            campoClave->text() == CLAVE_ADMIN) {
            pilaPantallas->setCurrentIndex(ADMIN);
        } else {
            QMessageBox::warning(this, "Acceso denegado",
                                 "Correo o contrasena incorrectos.");
        }
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

// Al cerrar el programa se guarda el estado ACTUAL en memoria (no el
// archivo de carga masiva) en los tres archivos comprimidos .edd.
void MainWindow::closeEvent(QCloseEvent* event) {
    if (!PersistenciaEDD::guardarTodo("..", grafoSedes, cartelera, arbolFunciones)) {
        QMessageBox::warning(this, "Persistencia",
            "No se pudieron escribir los archivos .edd (sedes, peliculas, funciones).");
    }
    QMainWindow::closeEvent(event);
}
