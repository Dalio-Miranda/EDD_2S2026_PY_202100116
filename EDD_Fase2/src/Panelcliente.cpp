#include "PanelCliente.h"
#include "PanelClienteCartelera.h"
#include "PanelClienteAsientos.h"
#include "PanelClienteCancelarReserva.h"
#include "PanelClientePromociones.h"
#include "PanelClienteSolicitudes.h"
#include "PanelClienteReservar2.h"
#include "PanelClienteHistorial2.h"
#include "PanelClienteCancelarReserva2.h"
#include "PanelClienteMiPerfil.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QFont>
#include <QMessageBox>
#include <QLayoutItem>

PanelCliente::PanelCliente(ArbolBinarioPeliculas& carteleraRef,
                            ListaCircularPromociones& promocionesRef,
                            ListaCircularSolicitudes& solicitudesRef,
                            MatrizDispersaAsientos& matrizAsientosRef,
                            ArbolAVLFunciones& arbolFunciones,
                            ArbolBClientes& arbolClientes,
                            TablaHashReservas& tablaReservas,
                            GestorAsientosPorFuncion& gestorAsientos,
                            QWidget* parent)
    : QWidget(parent), cartelera(carteleraRef), promociones(promocionesRef), solicitudes(solicitudesRef),
      matrizAsientos(matrizAsientosRef), funciones(arbolFunciones), clientes(arbolClientes),
      reservas(tablaReservas), gestor(gestorAsientos) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    pila = new QStackedWidget();
    paginaLogin = construirPaginaLogin();
    paginaSesion = new QWidget();

    pila->addWidget(paginaLogin);
    pila->addWidget(paginaSesion);

    layoutPrincipal->addWidget(pila);

    pila->setCurrentIndex(Pagina::LOGIN);
}

QWidget* PanelCliente::construirPaginaLogin() {
    QWidget* pagina = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(pagina);

    QHBoxLayout* layoutSuperior = new QHBoxLayout();
    QLabel* titulo = new QLabel("Acceso de Cliente");
    QFont fuenteTitulo = titulo->font();
    fuenteTitulo.setPointSize(16);
    fuenteTitulo.setBold(true);
    titulo->setFont(fuenteTitulo);

    QPushButton* botonVolver = new QPushButton("Volver al inicio");
    connect(botonVolver, &QPushButton::clicked, this, &PanelCliente::solicitudVolver);

    layoutSuperior->addWidget(titulo);
    layoutSuperior->addStretch();
    layoutSuperior->addWidget(botonVolver);
    layout->addLayout(layoutSuperior);

    QLabel* subtituloLogin = new QLabel("Iniciar sesion");
    QFont fuenteSubtitulo = subtituloLogin->font();
    fuenteSubtitulo.setBold(true);
    subtituloLogin->setFont(fuenteSubtitulo);
    layout->addWidget(subtituloLogin);

    QFormLayout* formLogin = new QFormLayout();
    campoLoginCorreo = new QLineEdit();
    campoLoginPassword = new QLineEdit();
    campoLoginPassword->setEchoMode(QLineEdit::Password);
    formLogin->addRow("Correo:", campoLoginCorreo);
    formLogin->addRow("Password:", campoLoginPassword);
    layout->addLayout(formLogin);

    QPushButton* botonLogin = new QPushButton("Iniciar Sesion");
    layout->addWidget(botonLogin);
    connect(botonLogin, &QPushButton::clicked, this, &PanelCliente::onLoginClicked);

    layout->addSpacing(20);

    QLabel* subtituloRegistro = new QLabel("Registrarse");
    subtituloRegistro->setFont(fuenteSubtitulo);
    layout->addWidget(subtituloRegistro);

    QFormLayout* formRegistro = new QFormLayout();
    QLineEdit* campoRegId = new QLineEdit();
    QLineEdit* campoRegNombre = new QLineEdit();
    QLineEdit* campoRegCorreo = new QLineEdit();
    QLineEdit* campoRegTelefono = new QLineEdit();
    QLineEdit* campoRegPassword = new QLineEdit();
    campoRegPassword->setEchoMode(QLineEdit::Password);

    formRegistro->addRow("ID:", campoRegId);
    formRegistro->addRow("Nombre:", campoRegNombre);
    formRegistro->addRow("Correo:", campoRegCorreo);
    formRegistro->addRow("Telefono:", campoRegTelefono);
    formRegistro->addRow("Password:", campoRegPassword);
    layout->addLayout(formRegistro);

    QPushButton* botonRegistro = new QPushButton("Registrarse");
    layout->addWidget(botonRegistro);

    connect(botonRegistro, &QPushButton::clicked, this, [this, campoRegId, campoRegNombre, campoRegCorreo,
                                                          campoRegTelefono, campoRegPassword]() {
        std::string id = campoRegId->text().trimmed().toStdString();
        std::string nombre = campoRegNombre->text().trimmed().toStdString();
        std::string correo = campoRegCorreo->text().trimmed().toStdString();
        std::string telefono = campoRegTelefono->text().trimmed().toStdString();
        std::string password = campoRegPassword->text().toStdString();

        if (id.empty() || nombre.empty() || telefono.empty() || password.empty()) {
            QMessageBox::warning(this, "Datos incompletos", "Todos los campos son obligatorios.");
            return;
        }
        if (correo.find('@') == std::string::npos) {
            QMessageBox::warning(this, "Correo invalido", "El correo debe contener '@'.");
            return;
        }

        // El correo debe ser unico. El Arbol B esta indexado por id,
        // no por correo, asi que se recorre el listado completo
        // (ArregloDinamico devuelto por listarClientes()).
        for (const Cliente& existente : clientes.listarClientes()) {
            if (existente.correo == correo) {
                QMessageBox::warning(this, "Correo en uso", "Ya existe una cuenta con ese correo.");
                return;
            }
        }

        Cliente nuevo(id, nombre, correo, telefono, password);
        bool ok = clientes.insertar(nuevo);
        if (!ok) {
            QMessageBox::warning(this, "ID en uso", "Ya existe un cliente con ese ID.");
            return;
        }

        QMessageBox::information(this, "Registro exitoso", "Cuenta creada correctamente. Ya puedes iniciar sesion.");
        campoRegId->clear();
        campoRegNombre->clear();
        campoRegCorreo->clear();
        campoRegTelefono->clear();
        campoRegPassword->clear();
    });

    layout->addStretch();
    return pagina;
}

void PanelCliente::onLoginClicked() {
    std::string correo = campoLoginCorreo->text().trimmed().toStdString();
    std::string password = campoLoginPassword->text().toStdString();

    if (correo.empty() || password.empty()) {
        QMessageBox::warning(this, "Datos incompletos", "Ingresa tu correo y tu password.");
        return;
    }

    std::string idEncontrado;
    bool encontrado = false;
    for (const Cliente& c : clientes.listarClientes()) {
        if (c.correo == correo && c.password == password) {
            idEncontrado = c.id;
            encontrado = true;
            break;
        }
    }

    if (!encontrado) {
        QMessageBox::warning(this, "Credenciales incorrectas", "Correo o password incorrectos.");
        return;
    }

    idClienteActual = idEncontrado;
    campoLoginPassword->clear();

    construirPaginaSesion();
    pila->setCurrentIndex(Pagina::SESION);
}

void PanelCliente::onCerrarSesion() {
    idClienteActual.clear();
    campoLoginCorreo->clear();
    pila->setCurrentIndex(Pagina::LOGIN);
}

void PanelCliente::construirPaginaSesion() {
    QLayout* layoutViejo = paginaSesion->layout();
    if (layoutViejo != nullptr) {
        QLayoutItem* item;
        while ((item = layoutViejo->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        delete layoutViejo;
    }

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(paginaSesion);

    QHBoxLayout* layoutSuperior = new QHBoxLayout();
    QLabel* titulo = new QLabel(QString("Panel Cliente - %1").arg(QString::fromStdString(idClienteActual)));
    QFont fuenteTitulo = titulo->font();
    fuenteTitulo.setPointSize(16);
    fuenteTitulo.setBold(true);
    titulo->setFont(fuenteTitulo);

    QPushButton* botonCerrarSesion = new QPushButton("Cerrar Sesion");
    connect(botonCerrarSesion, &QPushButton::clicked, this, &PanelCliente::onCerrarSesion);

    layoutSuperior->addWidget(titulo);
    layoutSuperior->addStretch();
    layoutSuperior->addWidget(botonCerrarSesion);

    QTabWidget* pestanas = new QTabWidget();
    pestanas->addTab(new PanelClienteCartelera(cartelera), "Cartelera");
    pestanas->addTab(new PanelClienteMiPerfil(clientes, idClienteActual), "Mi Perfil");
    pestanas->addTab(new PanelClienteReservar2(cartelera, funciones, gestor, reservas, clientes, idClienteActual), "Reservar");
    pestanas->addTab(new PanelClienteHistorial2(clientes, reservas, idClienteActual), "Mis Reservas");
    pestanas->addTab(new PanelClienteCancelarReserva2(clientes, reservas, funciones, gestor, idClienteActual), "Cancelar Reserva");
    pestanas->addTab(new PanelClientePromociones(promociones), "Promociones");
    pestanas->addTab(new PanelClienteSolicitudes(solicitudes), "Solicitudes");
    pestanas->addTab(new PanelClienteAsientos(matrizAsientos), "Reservar (Fase 1)");
    pestanas->addTab(new PanelClienteCancelarReserva(matrizAsientos), "Cancelar (Fase 1)");

    layoutPrincipal->addLayout(layoutSuperior);
    layoutPrincipal->addWidget(pestanas);
}
