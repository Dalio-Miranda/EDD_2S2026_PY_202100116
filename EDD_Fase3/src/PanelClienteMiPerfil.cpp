#include "PanelClienteMiPerfil.h"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QShowEvent>
#include <QFont>

PanelClienteMiPerfil::PanelClienteMiPerfil(ArbolBClientes& clientesRef,
                                            std::string idClienteRef,
                                            QWidget* parent)
    : QWidget(parent), clientes(clientesRef), idCliente(std::move(idClienteRef)) {

    QVBoxLayout* layout = new QVBoxLayout(this);

    QLabel* titulo = new QLabel("Editar Mi Perfil");
    QFont fuenteTitulo = titulo->font();
    fuenteTitulo.setBold(true);
    fuenteTitulo.setPointSize(13);
    titulo->setFont(fuenteTitulo);
    layout->addWidget(titulo);

    QFormLayout* formulario = new QFormLayout();

    QLabel* labelId = new QLabel(QString::fromStdString(idCliente));
    campoNombre = new QLineEdit();
    campoCorreo = new QLineEdit();
    campoTelefono = new QLineEdit();
    campoPasswordNueva = new QLineEdit();
    campoPasswordNueva->setEchoMode(QLineEdit::Password);
    campoPasswordNueva->setPlaceholderText("Dejar en blanco para no cambiarla");

    formulario->addRow("ID (no editable):", labelId);
    formulario->addRow("Nombre:", campoNombre);
    formulario->addRow("Correo:", campoCorreo);
    formulario->addRow("Telefono:", campoTelefono);
    formulario->addRow("Nueva Password:", campoPasswordNueva);

    layout->addLayout(formulario);

    QPushButton* botonGuardar = new QPushButton("Guardar Cambios");
    layout->addWidget(botonGuardar, 0, Qt::AlignLeft);
    layout->addStretch();

    connect(botonGuardar, &QPushButton::clicked, this, &PanelClienteMiPerfil::onGuardarCambios);
}

void PanelClienteMiPerfil::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    cargarDatosActuales();
}

void PanelClienteMiPerfil::cargarDatosActuales() {
    Cliente* c = clientes.buscar(idCliente);
    if (c == nullptr) return;

    campoNombre->setText(QString::fromStdString(c->nombre));
    campoCorreo->setText(QString::fromStdString(c->correo));
    campoTelefono->setText(QString::fromStdString(c->telefono));
    campoPasswordNueva->clear();
}

void PanelClienteMiPerfil::onGuardarCambios() {
    Cliente* c = clientes.buscar(idCliente);
    if (c == nullptr) return;

    std::string nombre = campoNombre->text().trimmed().toStdString();
    std::string correo = campoCorreo->text().trimmed().toStdString();
    std::string telefono = campoTelefono->text().trimmed().toStdString();
    std::string passwordNueva = campoPasswordNueva->text().toStdString();

    if (nombre.empty() || telefono.empty()) {
        QMessageBox::warning(this, "Datos incompletos", "Nombre y telefono no pueden estar vacios.");
        return;
    }
    if (correo.find('@') == std::string::npos) {
        QMessageBox::warning(this, "Correo invalido", "El correo debe contener '@'.");
        return;
    }

    // El correo debe seguir siendo unico entre TODOS los clientes,
    // excepto el propio (si no lo cambio).
    for (const Cliente& existente : clientes.listarClientes()) {
        if (existente.id != idCliente && existente.correo == correo) {
            QMessageBox::warning(this, "Correo en uso", "Ya existe otra cuenta con ese correo.");
            return;
        }
    }

    c->nombre = nombre;
    c->correo = correo;
    c->telefono = telefono;
    if (!passwordNueva.empty()) {
        c->password = passwordNueva;
    }

    campoPasswordNueva->clear();
    QMessageBox::information(this, "Perfil actualizado", "Tus datos se guardaron correctamente.");
}
