#include <QApplication>
#include "MainWindow.h"

// Hoja de estilos global (QSS, funciona como CSS) para darle a
// CinemaUSAC una apariencia oscura y profesional con acento dorado,
// en vez del gris plano por defecto de Qt.
static const char* HOJA_DE_ESTILOS = R"(
QWidget {
    background-color: #1b1e24;
    color: #eaeaea;
    font-family: "Segoe UI";
    font-size: 10pt;
}

QMainWindow {
    background-color: #1b1e24;
}

/* Los labels no deben pintar su propio fondo solido: si no, tapan
   cosas como la marca de agua del logo con un rectangulo visible. */
QLabel {
    background: transparent;
}

/* ---------- Botones ---------- */
QPushButton {
    background-color: #2a2f38;
    color: #eaeaea;
    border: 1px solid #3a4048;
    border-radius: 6px;
    padding: 8px 16px;
}
QPushButton:hover {
    background-color: #343b45;
    border-color: #E3B23C;
}
QPushButton:pressed {
    background-color: #E3B23C;
    color: #1b1e24;
    font-weight: bold;
}

/* Botones grandes de la pantalla de inicio */
QPushButton#botonEntrada {
    background-color: #262b33;
    border: 2px solid #E3B23C;
    color: #E3B23C;
    font-size: 12pt;
    font-weight: bold;
    border-radius: 8px;
}
QPushButton#botonEntrada:hover {
    background-color: #E3B23C;
    color: #1b1e24;
}

/* ---------- Titulos ---------- */
QLabel#tituloApp {
    color: #E3B23C;
}
QLabel#tituloPanel {
    color: #E3B23C;
}

/* ---------- Pestanas ---------- */
QTabWidget::pane {
    border: 1px solid #343a45;
    background: #20242c;
    border-radius: 6px;
    top: -1px;
}
QTabBar::tab {
    background: #20242c;
    color: #9aa0a6;
    padding: 8px 20px;
    border-top-left-radius: 6px;
    border-top-right-radius: 6px;
    margin-right: 2px;
}
QTabBar::tab:selected {
    background: #2a2f38;
    color: #E3B23C;
    font-weight: bold;
    border-bottom: 2px solid #E3B23C;
}
QTabBar::tab:hover:!selected {
    background: #262b33;
}

/* ---------- Tablas ---------- */
QTableWidget {
    background-color: #20242c;
    alternate-background-color: #242830;
    gridline-color: #343a45;
    border: 1px solid #343a45;
    border-radius: 6px;
    selection-background-color: #3a4048;
    selection-color: #E3B23C;
}
QHeaderView::section {
    background-color: #2a2f38;
    color: #E3B23C;
    padding: 6px;
    border: none;
    font-weight: bold;
}

/* ---------- Campos de entrada ---------- */
QLineEdit, QComboBox, QSpinBox, QDateEdit, QTextEdit {
    background-color: #262b33;
    border: 1px solid #3a4048;
    border-radius: 5px;
    padding: 5px;
    color: #eaeaea;
}
QLineEdit:focus, QComboBox:focus, QSpinBox:focus, QDateEdit:focus, QTextEdit:focus {
    border: 1px solid #E3B23C;
}
QComboBox::drop-down {
    border: none;
}

/* ---------- Grupos ---------- */
QGroupBox {
    border: 1px solid #343a45;
    border-radius: 6px;
    margin-top: 14px;
    padding-top: 12px;
    font-weight: bold;
    color: #E3B23C;
}
QGroupBox::title {
    subcontrol-origin: margin;
    left: 10px;
    padding: 0 5px;
}

/* ---------- Otros ---------- */
QScrollArea {
    border: none;
}
QCheckBox {
    spacing: 6px;
}
QDialog {
    background-color: #1b1e24;
}
)";

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setStyleSheet(HOJA_DE_ESTILOS);

    MainWindow ventana;
    ventana.show();

    return app.exec();
}