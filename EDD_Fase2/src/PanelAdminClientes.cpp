#include "PanelAdminClientes.h"
#include "PersistenciaJSON.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>
#include <QDialog>
#include <QFormLayout>
#include <QLineEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QFileDialog>
#include <QShowEvent>

PanelAdminClientes::PanelAdminClientes(ArbolBClientes& clientesRef,
                                        TablaHashReservas& reservasRef,
                                        QWidget* parent)
    : QWidget(parent), clientes(clientesRef), reservas(reservasRef) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    QHBoxLayout* layoutBotones = new QHBoxLayout();
    QPushButton* botonAgregar = new QPushButton("Agregar Cliente");
    QPushButton* botonEditar = new QPushButton("Editar Cliente");
    QPushButton* botonEliminar = new QPushButton("Eliminar Cliente");
    QPushButton* botonVerReservas = new QPushButton("Ver Reservas");
    QPushButton* botonJSON = new QPushButton("Cargar JSON (Clientes+Reservas)");
    layoutBotones->addWidget(botonAgregar);
    layoutBotones->addWidget(botonEditar);
    layoutBotones->addWidget(botonEliminar);
    layoutBotones->addWidget(botonVerReservas);
    layoutBotones->addWidget(botonJSON);
    layoutBotones->addStretch();

    QHBoxLayout* layoutRecorrido = new QHBoxLayout();
    layoutRecorrido->addWidget(new QLabel("Ordenar listado por recorrido:"));
    comboRecorrido = new QComboBox();
    comboRecorrido->addItem("Inorden (por ID, ascendente)");
    comboRecorrido->addItem("Preorden");
    comboRecorrido->addItem("Postorden");
    layoutRecorrido->addWidget(comboRecorrido);
    layoutRecorrido->addStretch();

    tablaClientes = new QTableWidget();
    tablaClientes->setColumnCount(6);
    tablaClientes->setHorizontalHeaderLabels({"ID", "Nombre", "Correo", "Telefono", "Password", "# Reservas"});
    tablaClientes->horizontalHeader()->setStretchLastSection(true);
    tablaClientes->setSelectionBehavior(QAbstractItemView::SelectRows);
    tablaClientes->setSelectionMode(QAbstractItemView::SingleSelection);
    tablaClientes->setEditTriggers(QAbstractItemView::NoEditTriggers);

    layoutPrincipal->addLayout(layoutBotones);
    layoutPrincipal->addLayout(layoutRecorrido);
    layoutPrincipal->addWidget(tablaClientes);

    connect(botonAgregar, &QPushButton::clicked, this, &PanelAdminClientes::onAgregarCliente);
    connect(botonEditar, &QPushButton::clicked, this, &PanelAdminClientes::onEditarCliente);
    connect(botonEliminar, &QPushButton::clicked, this, &PanelAdminClientes::onEliminarCliente);
    connect(botonVerReservas, &QPushButton::clicked, this, &PanelAdminClientes::onVerReservas);
    connect(botonJSON, &QPushButton::clicked, this, &PanelAdminClientes::onCargarJSON);
    connect(comboRecorrido, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &PanelAdminClientes::onCambioRecorrido);
}

void PanelAdminClientes::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    actualizarListaClientes();
}

void PanelAdminClientes::onCambioRecorrido() {
    actualizarListaClientes();
}

std::string PanelAdminClientes::idSeleccionado() const {
    auto seleccion = tablaClientes->selectionModel()->selectedRows();
    if (seleccion.isEmpty()) return "";
    int fila = seleccion.first().row();
    QTableWidgetItem* item = tablaClientes->item(fila, 0);
    return item != nullptr ? item->text().toStdString() : "";
}

void PanelAdminClientes::actualizarListaClientes() {
    TipoRecorrido tipo = TipoRecorrido::INORDEN;
    switch (comboRecorrido->currentIndex()) {
        case 1: tipo = TipoRecorrido::PREORDEN; break;
        case 2: tipo = TipoRecorrido::POSTORDEN; break;
        default: tipo = TipoRecorrido::INORDEN; break;
    }

    ArregloDinamico<Cliente> listado = clientes.listarClientes(tipo);

    tablaClientes->setRowCount(listado.getTamanio());
    for (int i = 0; i < listado.getTamanio(); i++) {
        const Cliente& c = listado[i];
        tablaClientes->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(c.id)));
        tablaClientes->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(c.nombre)));
        tablaClientes->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(c.correo)));
        tablaClientes->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(c.telefono)));
        tablaClientes->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(c.password)));
        tablaClientes->setItem(i, 5, new QTableWidgetItem(QString::number(c.codigosReserva.getTamanio())));
    }
}

void PanelAdminClientes::onAgregarCliente() {
    QDialog dialogo(this);
    dialogo.setWindowTitle("Agregar Cliente");
    QFormLayout* formulario = new QFormLayout(&dialogo);

    QLineEdit* campoId = new QLineEdit();
    QLineEdit* campoNombre = new QLineEdit();
    QLineEdit* campoCorreo = new QLineEdit();
    QLineEdit* campoTelefono = new QLineEdit();
    QLineEdit* campoPassword = new QLineEdit();
    campoPassword->setEchoMode(QLineEdit::Password);

    formulario->addRow("ID:", campoId);
    formulario->addRow("Nombre:", campoNombre);
    formulario->addRow("Correo:", campoCorreo);
    formulario->addRow("Telefono:", campoTelefono);
    formulario->addRow("Password:", campoPassword);

    QDialogButtonBox* botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formulario->addRow(botones);
    connect(botones, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);

    if (dialogo.exec() == QDialog::Accepted) {
        std::string id = campoId->text().trimmed().toStdString();
        std::string nombre = campoNombre->text().trimmed().toStdString();
        std::string correo = campoCorreo->text().trimmed().toStdString();
        std::string telefono = campoTelefono->text().trimmed().toStdString();
        std::string password = campoPassword->text().toStdString();

        if (id.empty() || nombre.empty() || telefono.empty() || password.empty()) {
            QMessageBox::warning(this, "Datos incompletos", "Todos los campos son obligatorios.");
            return;
        }
        if (correo.find('@') == std::string::npos) {
            QMessageBox::warning(this, "Correo invalido", "El correo debe contener '@'.");
            return;
        }

        Cliente c(id, nombre, correo, telefono, password);
        bool ok = clientes.insertar(c);
        if (!ok) {
            QMessageBox::warning(this, "ID duplicado", "Ya existe un cliente con ese ID.");
            return;
        }

        actualizarListaClientes();
        QMessageBox::information(this, "Cliente agregado", "El cliente se agrego correctamente.");
    }
}

void PanelAdminClientes::onEditarCliente() {
    std::string id = idSeleccionado();
    if (id.empty()) {
        QMessageBox::information(this, "Sin seleccion", "Selecciona un cliente de la tabla primero.");
        return;
    }

    Cliente* c = clientes.buscar(id);
    if (c == nullptr) return;

    QDialog dialogo(this);
    dialogo.setWindowTitle("Editar Cliente " + QString::fromStdString(id));
    QFormLayout* formulario = new QFormLayout(&dialogo);

    QLineEdit* campoNombre = new QLineEdit(QString::fromStdString(c->nombre));
    QLineEdit* campoCorreo = new QLineEdit(QString::fromStdString(c->correo));
    QLineEdit* campoTelefono = new QLineEdit(QString::fromStdString(c->telefono));

    formulario->addRow("ID (no editable):", new QLabel(QString::fromStdString(id)));
    formulario->addRow("Nombre:", campoNombre);
    formulario->addRow("Correo:", campoCorreo);
    formulario->addRow("Telefono:", campoTelefono);

    QDialogButtonBox* botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formulario->addRow(botones);
    connect(botones, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);

    if (dialogo.exec() == QDialog::Accepted) {
        std::string nombre = campoNombre->text().trimmed().toStdString();
        std::string correo = campoCorreo->text().trimmed().toStdString();
        std::string telefono = campoTelefono->text().trimmed().toStdString();

        if (nombre.empty() || telefono.empty()) {
            QMessageBox::warning(this, "Datos incompletos", "Nombre y telefono no pueden estar vacios.");
            return;
        }
        if (correo.find('@') == std::string::npos) {
            QMessageBox::warning(this, "Correo invalido", "El correo debe contener '@'.");
            return;
        }

        c->nombre = nombre;
        c->correo = correo;
        c->telefono = telefono;

        actualizarListaClientes();
        QMessageBox::information(this, "Cliente editado", "Los cambios se guardaron correctamente.");
    }
}

void PanelAdminClientes::onEliminarCliente() {
    std::string id = idSeleccionado();
    if (id.empty()) {
        QMessageBox::information(this, "Sin seleccion", "Selecciona un cliente de la tabla primero.");
        return;
    }

    Cliente* c = clientes.buscar(id);
    if (c == nullptr) return;

    int cantidadReservas = c->codigosReserva.getTamanio();
    auto respuesta = QMessageBox::question(this, "Confirmar eliminacion",
        QString("Deseas eliminar al cliente %1?\n\nSe eliminaran tambien sus %2 reserva(s) asociada(s).")
            .arg(QString::fromStdString(id)).arg(cantidadReservas));

    if (respuesta != QMessageBox::Yes) return;

    // Copia (ArregloDinamico soporta copia profunda) antes de
    // eliminar el cliente, porque el puntero c queda invalido
    // despues de arbolClientes.eliminar().
    ArregloDinamico<std::string> codigosAEliminar = c->codigosReserva;
    for (const std::string& codigo : codigosAEliminar) {
        reservas.eliminar(codigo);
    }

    clientes.eliminar(id);

    actualizarListaClientes();
    QMessageBox::information(this, "Cliente eliminado",
        "El cliente y sus reservas asociadas se eliminaron correctamente.");
}

void PanelAdminClientes::onVerReservas() {
    std::string id = idSeleccionado();
    if (id.empty()) {
        QMessageBox::information(this, "Sin seleccion", "Selecciona un cliente de la tabla primero.");
        return;
    }

    Cliente* c = clientes.buscar(id);
    if (c == nullptr) return;

    if (c->codigosReserva.estaVacio()) {
        QMessageBox::information(this, "Sin reservas",
            QString("El cliente %1 no tiene reservas registradas.").arg(QString::fromStdString(id)));
        return;
    }

    std::string hoy = PersistenciaJSON::fechaHoy();

    QDialog dialogo(this);
    dialogo.setWindowTitle("Reservas de " + QString::fromStdString(id));
    dialogo.resize(500, 300);
    QVBoxLayout* layout = new QVBoxLayout(&dialogo);

    QTableWidget* tabla = new QTableWidget();
    tabla->setColumnCount(6);
    tabla->setHorizontalHeaderLabels({"Codigo", "Funcion", "Fecha", "Fila", "Columna", "Estado"});
    tabla->horizontalHeader()->setStretchLastSection(true);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setRowCount(c->codigosReserva.getTamanio());

    

    for (int i = 0; i < c->codigosReserva.getTamanio(); i++) {
        const std::string& codigo = c->codigosReserva[i];
        Reserva* r = reservas.buscar(codigo);

        if (r == nullptr) {
            tabla->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(codigo)));
            tabla->setItem(i, 1, new QTableWidgetItem("(no encontrada)"));
            continue;
        }

        std::string estado = (r->fechaReserva < hoy) ? "VENCIDA" : "VALIDA";

        tabla->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(r->codigoReserva)));
        tabla->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(r->codigoFuncion)));
        tabla->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(r->fechaReserva)));
        tabla->setItem(i, 3, new QTableWidgetItem(QString::number(r->fila)));
        tabla->setItem(i, 4, new QTableWidgetItem(QString::number(r->columna)));
        tabla->setItem(i, 5, new QTableWidgetItem(QString::fromStdString(estado)));
    }

    layout->addWidget(tabla);
    QDialogButtonBox* botonCerrar = new QDialogButtonBox(QDialogButtonBox::Close);
    connect(botonCerrar, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);
    connect(botonCerrar, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
    layout->addWidget(botonCerrar);

    dialogo.exec();
}

void PanelAdminClientes::onCargarJSON() {
    QString ruta = QFileDialog::getOpenFileName(this, "Selecciona el archivo JSON de clientes y reservas",
                                                 QString(), "Archivos JSON (*.json)");
    if (ruta.isEmpty()) return;

    ResultadoCarga r = PersistenciaJSON::cargarClientesYReservas(ruta.toStdString(), clientes, reservas);

    if (r.huboErrorFatal) {
        QMessageBox::critical(this, "Error al leer el archivo",
            r.errores.estaVacio() ? "No se pudo procesar el archivo." : QString::fromStdString(r.errores[0]));
        return;
    }

    QString resumen = QString("Carga completada.\n\nExitosos: %1\nRechazados: %2")
        .arg(r.exitosos).arg(r.rechazados);

    if (!r.errores.estaVacio()) {
        resumen += "\n\nDetalle:\n";
        int mostrados = 0;
        for (const auto& e : r.errores) {
            if (mostrados >= 15) {
                resumen += QString("... y %1 mas.").arg(r.errores.getTamanio() - mostrados);
                break;
            }
            resumen += "- " + QString::fromStdString(e) + "\n";
            mostrados++;
        }
    }

    QMessageBox::information(this, "Resultado de la carga masiva", resumen);
    actualizarListaClientes();
}