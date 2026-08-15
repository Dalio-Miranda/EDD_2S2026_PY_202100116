#include "PanelAdminFunciones.h"
#include "ReportesGraphviz.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDialog>
#include <QFormLayout>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QScrollArea>
#include <QDesktopServices>
#include <QUrl>
#include <QFont>
#include <QShowEvent>
#include <vector>

// Recolecta las peliculas de la cartelera (recorrido inorden) para
// llenar el combo de seleccion al crear una funcion.
static void recolectarPeliculasLocal(NodoArbolPelicula* nodo, std::vector<Pelicula>& resultado) {
    if (nodo == nullptr) return;
    recolectarPeliculasLocal(nodo->izquierdo, resultado);
    resultado.push_back(nodo->dato);
    recolectarPeliculasLocal(nodo->derecho, resultado);
}

PanelAdminFunciones::PanelAdminFunciones(MatrizDispersaAsientos& matrizRef, ArbolBinarioPeliculas& carteleraRef, QWidget* parent)
    : QWidget(parent), matriz(matrizRef), cartelera(carteleraRef) {

    QVBoxLayout* layoutPrincipal = new QVBoxLayout(this);

    QHBoxLayout* layoutBotones = new QHBoxLayout();
    QPushButton* botonCrear = new QPushButton("Crear Funcion");
    QPushButton* botonReporte = new QPushButton("Generar Reporte Graphviz");
    layoutBotones->addWidget(botonCrear);
    layoutBotones->addWidget(botonReporte);
    layoutBotones->addStretch();

    labelInfo = new QLabel("No hay ninguna funcion creada todavia.");
    QFont fuenteInfo = labelInfo->font();
    fuenteInfo.setBold(true);
    labelInfo->setFont(fuenteInfo);

    QLabel* leyenda = new QLabel("Verde = libre | Rojo = ocupado (clic para ver/eliminar la reserva)");

    contenedorGrilla = new QWidget();
    layoutGrilla = new QGridLayout(contenedorGrilla);
    layoutGrilla->setSpacing(4);

    QScrollArea* scroll = new QScrollArea();
    scroll->setWidget(contenedorGrilla);
    scroll->setWidgetResizable(true);

    layoutPrincipal->addLayout(layoutBotones);
    layoutPrincipal->addWidget(labelInfo);
    layoutPrincipal->addWidget(leyenda);
    layoutPrincipal->addWidget(scroll);

    connect(botonCrear, &QPushButton::clicked, this, &PanelAdminFunciones::onCrearFuncion);
    connect(botonReporte, &QPushButton::clicked, this, &PanelAdminFunciones::onGenerarReporte);
}

void PanelAdminFunciones::showEvent(QShowEvent* event) {
    QWidget::showEvent(event);
    dibujarGrilla(); // refresca por si el cliente reservo/cancelo mientras tanto
}

void PanelAdminFunciones::limpiarGrilla() {
    QLayoutItem* item;
    while ((item = layoutGrilla->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

void PanelAdminFunciones::dibujarGrilla() {
    limpiarGrilla();

    if (!matriz.existeFuncion()) {
        labelInfo->setText("No hay ninguna funcion creada todavia.");
        return;
    }

    labelInfo->setText(QString("Funcion: %1 - %2 - %3  |  Reservados: %4  |  Libres: %5  |  Total: %6")
        .arg(QString::fromStdString(matriz.getPelicula()))
        .arg(QString::fromStdString(matriz.getHorario()))
        .arg(QString::fromStdString(matriz.getSala()))
        .arg(matriz.getTotalReservados())
        .arg(matriz.getTotalLibres())
        .arg(matriz.getTotalAsientos()));

    int filas = matriz.getNumFilas();
    int columnas = matriz.getNumColumnas();

    // Encabezados de columna
    for (int c = 1; c <= columnas; c++) {
        QLabel* etiqueta = new QLabel(QString::number(c));
        etiqueta->setAlignment(Qt::AlignCenter);
        layoutGrilla->addWidget(etiqueta, 0, c);
    }

    for (int f = 1; f <= filas; f++) {
        QLabel* etiquetaFila = new QLabel(QString::number(f));
        etiquetaFila->setAlignment(Qt::AlignCenter);
        layoutGrilla->addWidget(etiquetaFila, f, 0);

        for (int c = 1; c <= columnas; c++) {
            bool ocupado = matriz.estaOcupado(f, c);
            QString cliente = QString::fromStdString(matriz.getClienteEnAsiento(f, c));

            QPushButton* boton = new QPushButton(ocupado ? cliente.left(3) : "");
            boton->setFixedSize(45, 35);
            boton->setToolTip(ocupado ? ("Reservado por: " + cliente) : "Libre");
            boton->setStyleSheet(ocupado
                ? "background-color: #F4A6A6;"
                : "background-color: #8BE28B;");

            connect(boton, &QPushButton::clicked, this, [this, f, c]() {
                onClickAsiento(f, c);
            });

            layoutGrilla->addWidget(boton, f, c);
        }
    }
}

void PanelAdminFunciones::onClickAsiento(int fila, int columna) {
    if (matriz.estaOcupado(fila, columna)) {
        QString cliente = QString::fromStdString(matriz.getClienteEnAsiento(fila, columna));
        auto respuesta = QMessageBox::question(this, "Asiento ocupado",
            QString("Fila %1, Columna %2 - Reservado por: %3\n\nDeseas eliminar esta reserva?")
                .arg(fila).arg(columna).arg(cliente));

        if (respuesta == QMessageBox::Yes) {
            matriz.cancelarAsiento(fila, columna);
            dibujarGrilla();
        }
    } else {
        QMessageBox::information(this, "Asiento libre",
            QString("Fila %1, Columna %2 esta libre.").arg(fila).arg(columna));
    }
}

void PanelAdminFunciones::onCrearFuncion() {
    std::vector<Pelicula> peliculas;
    recolectarPeliculasLocal(cartelera.getRaiz(), peliculas);

    if (peliculas.empty()) {
        QMessageBox::warning(this, "Sin peliculas", "Primero agrega peliculas a la cartelera.");
        return;
    }

    QDialog dialogo(this);
    dialogo.setWindowTitle("Crear Funcion");
    QFormLayout* formulario = new QFormLayout(&dialogo);

    QComboBox* campoPelicula = new QComboBox();
    for (const Pelicula& p : peliculas) {
        campoPelicula->addItem(QString::fromStdString(p.codigo + " - " + p.titulo));
    }

    QLineEdit* campoHorario = new QLineEdit("17:00");
    QLineEdit* campoSala = new QLineEdit("Sala 1");

    QSpinBox* campoFilas = new QSpinBox();
    campoFilas->setRange(1, 30);
    campoFilas->setValue(10);

    QSpinBox* campoColumnas = new QSpinBox();
    campoColumnas->setRange(1, 30);
    campoColumnas->setValue(15);

    formulario->addRow("Pelicula:", campoPelicula);
    formulario->addRow("Horario:", campoHorario);
    formulario->addRow("Sala:", campoSala);
    formulario->addRow("Numero de filas:", campoFilas);
    formulario->addRow("Numero de columnas:", campoColumnas);

    QDialogButtonBox* botones = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    formulario->addRow(botones);
    connect(botones, &QDialogButtonBox::accepted, &dialogo, &QDialog::accept);
    connect(botones, &QDialogButtonBox::rejected, &dialogo, &QDialog::reject);

    if (dialogo.exec() == QDialog::Accepted) {
        if (matriz.existeFuncion()) {
            auto respuesta = QMessageBox::question(this, "Sobreescribir funcion",
                "Ya existe una funcion activa. Crear una nueva la sobreescribira (Fase 1 solo permite una funcion a la vez). Continuar?");
            if (respuesta != QMessageBox::Yes) return;
        }

        matriz.crearFuncion(
            campoPelicula->currentText().toStdString(),
            campoHorario->text().trimmed().toStdString(),
            campoSala->text().trimmed().toStdString(),
            campoFilas->value(),
            campoColumnas->value()
        );

        dibujarGrilla();
        QMessageBox::information(this, "Funcion creada", "La funcion se creo correctamente.");
    }
}

void PanelAdminFunciones::onGenerarReporte() {
    if (!matriz.existeFuncion()) {
        QMessageBox::warning(this, "Sin funcion", "Primero crea una funcion.");
        return;
    }

    ReportesGraphviz::generarReporteMatrizAsientos(matriz, "../reports");

    auto respuesta = QMessageBox::information(this, "Reporte generado",
        "El reporte se genero en la carpeta reports/. Deseas abrirlo ahora?",
        QMessageBox::Yes | QMessageBox::No);

    if (respuesta == QMessageBox::Yes) {
        QDesktopServices::openUrl(QUrl::fromLocalFile("../reports/reporte4_matriz_asientos.png"));
    }
}