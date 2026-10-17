#include "PanelAdminSedes.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTabWidget>
#include <QPushButton>
#include <QDialog>
#include <QDialogButtonBox>
#include <QSpinBox>
#include <QMessageBox>
#include <QHeaderView>
#include <QShowEvent>

namespace {

QTableWidget* crearTabla(const QStringList& encabezados) {
    QTableWidget* tabla = new QTableWidget();
    tabla->setColumnCount(encabezados.size());
    tabla->setHorizontalHeaderLabels(encabezados);
    tabla->horizontalHeader()->setStretchLastSection(true);
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tabla->setSelectionBehavior(QAbstractItemView::SelectRows);
    return tabla;
}

} // namespace

PanelAdminSedes::PanelAdminSedes(GrafoSedes& grafoRef,
                                 ArbolAVLFunciones& funcionesRef,
                                 ArbolBinarioPeliculas& carteleraRef,
                                 GestorAsientosPorFuncion& gestorRef,
                                 QWidget* parent)
    : QWidget(parent), grafo(grafoRef), funciones(funcionesRef), cartelera(carteleraRef), gestor(gestorRef) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);
    QTabWidget* pestanas = new QTabWidget();

    // ---------------- Sub-pestana 1: listado + registrar ----------------
    QWidget* paginaSedes = new QWidget();
    QVBoxLayout* layoutSedes = new QVBoxLayout(paginaSedes);

    QPushButton* botonRegistrar = new QPushButton("Registrar Nueva Sede");
    QPushButton* botonEditar = new QPushButton("Editar Sede Seleccionada");
    QPushButton* botonEliminar = new QPushButton("Eliminar Sede Seleccionada");
    botonEditar->setObjectName("botonEditarSede");
    botonEliminar->setObjectName("botonEliminarSede");
    QHBoxLayout* filaBoton = new QHBoxLayout();
    filaBoton->addWidget(botonRegistrar);
    filaBoton->addWidget(botonEditar);
    filaBoton->addWidget(botonEliminar);
    filaBoton->addStretch();

    tablaSedes = crearTabla({"Codigo", "Nombre", "Direccion", "Salas", "Telefono", "# Conexiones"});
    tablaSedes->setObjectName("tablaSedes");

    layoutSedes->addLayout(filaBoton);
    layoutSedes->addWidget(tablaSedes);
    pestanas->addTab(paginaSedes, "Sedes");

    // ---------------- Sub-pestana 2: peliculas por sede ----------------
    QWidget* paginaPeliculas = new QWidget();
    QVBoxLayout* layoutPeliculas = new QVBoxLayout(paginaPeliculas);

    QHBoxLayout* filaConsulta = new QHBoxLayout();
    filaConsulta->addWidget(new QLabel("Codigo de la sede:"));
    campoCodigoSedeConsulta = new QLineEdit();
    campoCodigoSedeConsulta->setPlaceholderText("Ej: S001");
    campoCodigoSedeConsulta->setMaximumWidth(140);
    QPushButton* botonVerPeliculas = new QPushButton("Ver Peliculas");
    filaConsulta->addWidget(campoCodigoSedeConsulta);
    filaConsulta->addWidget(botonVerPeliculas);
    filaConsulta->addStretch();

    labelSedeConsulta = new QLabel("Ingresa el codigo de una sede para ver que se proyecta ahi.");
    tablaPeliculasPorSede = crearTabla({"Pelicula", "Funcion", "Fecha", "Horario", "Sala"});
    tablaPeliculasPorSede->setObjectName("tablaPeliculasPorSede");

    layoutPeliculas->addLayout(filaConsulta);
    layoutPeliculas->addWidget(labelSedeConsulta);
    layoutPeliculas->addWidget(tablaPeliculasPorSede);
    pestanas->addTab(paginaPeliculas, "Peliculas por sede");

    // ---------------- Sub-pestana 3: BFS ----------------
    QWidget* paginaBfs = new QWidget();
    QVBoxLayout* layoutBfs = new QVBoxLayout(paginaBfs);

    QHBoxLayout* filaBfs = new QHBoxLayout();
    comboOrigen = new QComboBox();
    comboDestino = new QComboBox();
    comboOrigen->setObjectName("comboOrigen");
    comboDestino->setObjectName("comboDestino");
    QPushButton* botonBfs = new QPushButton("Buscar Camino Mas Corto");
    filaBfs->addWidget(new QLabel("Origen:"));
    filaBfs->addWidget(comboOrigen);
    filaBfs->addWidget(new QLabel("Destino:"));
    filaBfs->addWidget(comboDestino);
    filaBfs->addWidget(botonBfs);
    filaBfs->addStretch();

    labelResultadoCamino = new QLabel("Elige dos sedes y presiona el boton.");
    labelResultadoCamino->setWordWrap(true);
    labelResultadoCamino->setObjectName("labelResultadoCamino");

    layoutBfs->addLayout(filaBfs);
    layoutBfs->addWidget(labelResultadoCamino);
    layoutBfs->addStretch();
    pestanas->addTab(paginaBfs, "Camino mas corto (BFS)");

    // ---------------- Sub-pestana 4: ranking ----------------
    QWidget* paginaRanking = new QWidget();
    QVBoxLayout* layoutRanking = new QVBoxLayout(paginaRanking);

    QPushButton* botonRanking = new QPushButton("Generar Ranking de Aristas");
    QHBoxLayout* filaRanking = new QHBoxLayout();
    filaRanking->addWidget(botonRanking);
    filaRanking->addStretch();

    tablaRanking = crearTabla({"#", "Sede A", "Sede B", "Peliculas en comun (peso)"});
    tablaRanking->setObjectName("tablaRanking");

    layoutRanking->addLayout(filaRanking);
    layoutRanking->addWidget(tablaRanking);
    pestanas->addTab(paginaRanking, "Ranking de relacion");

    layoutPrincipal->addWidget(pestanas);

    connect(botonRegistrar, &QPushButton::clicked, this, &PanelAdminSedes::onRegistrarSede);
    connect(botonEditar, &QPushButton::clicked, this, &PanelAdminSedes::onEditarSede);
    connect(botonEliminar, &QPushButton::clicked, this, &PanelAdminSedes::onEliminarSede);
    connect(botonVerPeliculas, &QPushButton::clicked, this, &PanelAdminSedes::onVerPeliculasPorSede);
    connect(campoCodigoSedeConsulta, &QLineEdit::returnPressed, this, &PanelAdminSedes::onVerPeliculasPorSede);
    connect(botonBfs, &QPushButton::clicked, this, &PanelAdminSedes::onBuscarCamino);
    connect(botonRanking, &QPushButton::clicked, this, &PanelAdminSedes::onGenerarRanking);
}

void PanelAdminSedes::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    actualizarTablaSedes();
    actualizarCombosSedes();
}

void PanelAdminSedes::actualizarTablaSedes() {
    ArregloDinamico<Sede> sedes = grafo.listarSedes();
    tablaSedes->setRowCount(sedes.getTamanio());

    for (int i = 0; i < sedes.getTamanio(); i++) {
        const Sede& s = sedes[i];
        int conexiones = grafo.sedesSimilares(s.codigo).getTamanio();

        tablaSedes->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(s.codigo)));
        tablaSedes->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(s.nombre)));
        tablaSedes->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(s.direccion)));
        tablaSedes->setItem(i, 3, new QTableWidgetItem(QString::number(s.cantidadSalas)));
        tablaSedes->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(s.telefono)));
        tablaSedes->setItem(i, 5, new QTableWidgetItem(QString::number(conexiones)));
    }
}

void PanelAdminSedes::actualizarCombosSedes() {
    QString origenPrevio = comboOrigen->currentData().toString();
    QString destinoPrevio = comboDestino->currentData().toString();

    comboOrigen->clear();
    comboDestino->clear();

    ArregloDinamico<Sede> sedes = grafo.listarSedes();
    for (const Sede& s : sedes) {
        QString etiqueta = QString::fromStdString(s.codigo + " - " + s.nombre);
        comboOrigen->addItem(etiqueta, QString::fromStdString(s.codigo));
        comboDestino->addItem(etiqueta, QString::fromStdString(s.codigo));
    }

    int io = comboOrigen->findData(origenPrevio);
    if (io >= 0) comboOrigen->setCurrentIndex(io);
    int id = comboDestino->findData(destinoPrevio);
    if (id >= 0) comboDestino->setCurrentIndex(id);
}

void PanelAdminSedes::onRegistrarSede() {
    QDialog dialogo(this);
    dialogo.setWindowTitle("Registrar Nueva Sede");
    QFormLayout* formulario = new QFormLayout(&dialogo);

    QLineEdit* campoCodigo = new QLineEdit();
    campoCodigo->setPlaceholderText("Ej: S005");
    QLineEdit* campoNombre = new QLineEdit();
    QLineEdit* campoDireccion = new QLineEdit();
    QSpinBox* campoSalas = new QSpinBox();
    campoSalas->setRange(1, 50);
    campoSalas->setValue(3);
    QLineEdit* campoTelefono = new QLineEdit();
    campoTelefono->setPlaceholderText("Ej: 1122-3344");

    formulario->addRow("Codigo:", campoCodigo);
    formulario->addRow("Nombre:", campoNombre);
    formulario->addRow("Direccion:", campoDireccion);
    formulario->addRow("Cantidad de salas:", campoSalas);
    formulario->addRow("Telefono:", campoTelefono);

    QDialogButtonBox* botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formulario->addRow(botones);
    connect(botones, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);

    if (dialogo.exec() != QDialog::Accepted) return;

    std::string codigo = campoCodigo->text().trimmed().toStdString();
    std::string nombre = campoNombre->text().trimmed().toStdString();
    std::string direccion = campoDireccion->text().trimmed().toStdString();
    std::string telefono = campoTelefono->text().trimmed().toStdString();

    if (codigo.empty() || nombre.empty() || direccion.empty() || telefono.empty()) {
        QMessageBox::warning(this, "Datos incompletos", "Todos los campos son obligatorios.");
        return;
    }
    if (grafo.existeSede(codigo)) {
        QMessageBox::warning(this, "Codigo duplicado", "Ya existe una sede con ese codigo.");
        return;
    }

    // La sede entra al grafo como un vertice NUEVO, sin aristas: las
    // conexiones se generan solas cuando se creen funciones que
    // compartan peliculas con otras sedes.
    grafo.agregarSede(Sede(codigo, nombre, direccion, campoSalas->value(), telefono));

    actualizarTablaSedes();
    actualizarCombosSedes();
    QMessageBox::information(this, "Sede registrada",
        QString("La sede %1 se agrego al grafo como un vertice nuevo (sin conexiones todavia).")
            .arg(QString::fromStdString(codigo)));
}

std::string PanelAdminSedes::codigoSedeSeleccionada() const {
    auto seleccion = tablaSedes->selectionModel()->selectedRows();
    if (seleccion.isEmpty()) return "";
    QTableWidgetItem* item = tablaSedes->item(seleccion.first().row(), 0);
    return item != nullptr ? item->text().toStdString() : "";
}

void PanelAdminSedes::onEditarSede() {
    std::string codigo = codigoSedeSeleccionada();
    if (codigo.empty()) {
        QMessageBox::information(this, "Sin seleccion", "Selecciona una sede de la tabla primero.");
        return;
    }
    Sede* sede = grafo.buscarSede(codigo);
    if (sede == nullptr) return;

    QDialog dialogo(this);
    dialogo.setWindowTitle("Editar Sede");
    QFormLayout* formulario = new QFormLayout(&dialogo);

    // El codigo no se puede cambiar: las funciones lo usan para referenciar la sede.
    QLineEdit* campoCodigo = new QLineEdit(QString::fromStdString(sede->codigo));
    campoCodigo->setReadOnly(true);
    QLineEdit* campoNombre = new QLineEdit(QString::fromStdString(sede->nombre));
    QLineEdit* campoDireccion = new QLineEdit(QString::fromStdString(sede->direccion));
    QSpinBox* campoSalas = new QSpinBox();
    campoSalas->setRange(1, 50);
    campoSalas->setValue(sede->cantidadSalas);
    QLineEdit* campoTelefono = new QLineEdit(QString::fromStdString(sede->telefono));

    formulario->addRow("Codigo (no editable):", campoCodigo);
    formulario->addRow("Nombre:", campoNombre);
    formulario->addRow("Direccion:", campoDireccion);
    formulario->addRow("Cantidad de salas:", campoSalas);
    formulario->addRow("Telefono:", campoTelefono);

    QDialogButtonBox* botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formulario->addRow(botones);
    connect(botones, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);

    if (dialogo.exec() != QDialog::Accepted) return;

    std::string nombre = campoNombre->text().trimmed().toStdString();
    std::string direccion = campoDireccion->text().trimmed().toStdString();
    std::string telefono = campoTelefono->text().trimmed().toStdString();
    if (nombre.empty() || direccion.empty() || telefono.empty()) {
        QMessageBox::warning(this, "Datos incompletos", "Todos los campos son obligatorios.");
        return;
    }

    grafo.editarSede(codigo, nombre, direccion, campoSalas->value(), telefono);
    actualizarTablaSedes();
    actualizarCombosSedes();
    QMessageBox::information(this, "Sede actualizada",
        QString("Los datos de la sede %1 se actualizaron.").arg(QString::fromStdString(codigo)));
}

void PanelAdminSedes::onEliminarSede() {
    std::string codigo = codigoSedeSeleccionada();
    if (codigo.empty()) {
        QMessageBox::information(this, "Sin seleccion", "Selecciona una sede de la tabla primero.");
        return;
    }

    // Funciones que se proyectan en esta sede: al desaparecer la sede
    // no pueden quedar funciones apuntando a ella, asi que tambien se eliminan.
    ArregloDinamico<Funcion> todas = funciones.listarFunciones(TipoRecorrido::INORDEN);
    ArregloDinamico<std::string> codigosFunciones;
    for (const Funcion& f : todas) {
        if (f.codigoSede == codigo) codigosFunciones.agregarFinal(f.codigoFuncion);
    }

    QString mensaje = QString("Deseas eliminar la sede %1?\n\nSe quitaran tambien todas sus conexiones del grafo.")
                          .arg(QString::fromStdString(codigo));
    if (!codigosFunciones.estaVacio()) {
        mensaje += QString("\n\nLa sede tiene %1 funcion(es) que tambien se eliminaran (con sus archivos de asientos).")
                       .arg(codigosFunciones.getTamanio());
    }
    if (QMessageBox::question(this, "Confirmar eliminacion", mensaje) != QMessageBox::Yes) return;

    for (const std::string& codigoFuncion : codigosFunciones) {
        gestor.eliminarArchivoFuncion(codigoFuncion);
        funciones.eliminar(codigoFuncion);
    }
    grafo.eliminarSede(codigo);

    actualizarTablaSedes();
    actualizarCombosSedes();
    QMessageBox::information(this, "Sede eliminada",
        QString("La sede %1 y sus conexiones se eliminaron del grafo.").arg(QString::fromStdString(codigo)));
}

void PanelAdminSedes::onVerPeliculasPorSede() {
    std::string codigo = campoCodigoSedeConsulta->text().trimmed().toStdString();
    if (codigo.empty()) {
        QMessageBox::information(this, "Falta el codigo", "Ingresa el codigo de una sede (ej. S001).");
        return;
    }

    Sede* sede = grafo.buscarSede(codigo);
    if (sede == nullptr) {
        tablaPeliculasPorSede->setRowCount(0);
        labelSedeConsulta->setText("No existe ninguna sede con codigo '" + QString::fromStdString(codigo) + "'.");
        return;
    }

    // Consulta directa sobre la informacion del vertice: se recorren
    // las funciones asociadas a esta sede (sin necesidad de recorrer
    // aristas).
    ArregloDinamico<Funcion> todas = funciones.listarFunciones(TipoRecorrido::INORDEN);
    ArregloDinamico<Funcion> enSede;
    for (const Funcion& f : todas) {
        if (f.codigoSede == codigo) enSede.agregarFinal(f);
    }

    tablaPeliculasPorSede->setRowCount(enSede.getTamanio());
    for (int i = 0; i < enSede.getTamanio(); i++) {
        const Funcion& f = enSede[i];
        std::string titulo = f.codigoPelicula;
        Pelicula* p = cartelera.buscar(f.codigoPelicula);
        if (p != nullptr) titulo = p->codigo + " - " + p->titulo;

        tablaPeliculasPorSede->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(titulo)));
        tablaPeliculasPorSede->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(f.codigoFuncion)));
        tablaPeliculasPorSede->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(f.fecha)));
        tablaPeliculasPorSede->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(f.horario)));
        tablaPeliculasPorSede->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(f.sala)));
    }

    labelSedeConsulta->setText(QString("%1 - %2: %3 funcion(es) programada(s)")
        .arg(QString::fromStdString(sede->codigo))
        .arg(QString::fromStdString(sede->nombre))
        .arg(enSede.getTamanio()));
}

void PanelAdminSedes::onBuscarCamino() {
    if (comboOrigen->count() == 0) {
        labelResultadoCamino->setText("No hay sedes registradas todavia.");
        return;
    }

    std::string origen = comboOrigen->currentData().toString().toStdString();
    std::string destino = comboDestino->currentData().toString().toStdString();

    ArregloDinamico<std::string> camino = grafo.caminoMasCorto(origen, destino);

    if (camino.estaVacio()) {
        labelResultadoCamino->setText(QString("No existe un camino entre %1 y %2: no estan conectadas, "
                                              "ni directa ni indirectamente, por peliculas compartidas.")
            .arg(QString::fromStdString(origen)).arg(QString::fromStdString(destino)));
        return;
    }

    QString ruta;
    for (int i = 0; i < camino.getTamanio(); i++) {
        ruta += QString::fromStdString(camino[i]);
        if (i + 1 < camino.getTamanio()) ruta += "  ->  ";
    }

    int saltos = camino.getTamanio() - 1;
    labelResultadoCamino->setText(QString("Camino mas corto (%1 salto(s), %2 sede(s) intermedia(s)):\n\n%3")
        .arg(saltos)
        .arg(saltos > 0 ? saltos - 1 : 0)
        .arg(ruta));
}

void PanelAdminSedes::onGenerarRanking() {
    ArregloDinamico<ParSedes> ranking = grafo.listarAristasOrdenadas();
    tablaRanking->setRowCount(ranking.getTamanio());

    for (int i = 0; i < ranking.getTamanio(); i++) {
        const ParSedes& par = ranking[i];
        tablaRanking->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        tablaRanking->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(par.sedeA)));
        tablaRanking->setItem(i, 2, new QTableWidgetItem(QString::fromStdString(par.sedeB)));
        tablaRanking->setItem(i, 3, new QTableWidgetItem(QString::number(par.peso)));
    }

    if (ranking.estaVacio()) {
        QMessageBox::information(this, "Sin conexiones",
            "Todavia no hay aristas en el grafo: ninguna pareja de sedes comparte peliculas.");
    }
}
