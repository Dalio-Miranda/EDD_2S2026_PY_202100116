#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>

#include "ArbolBinarioPeliculas.h"
#include "ListaCircularPromociones.h"
#include "ListaCircularSolicitudes.h"
#include "MatrizDispersaAsientos.h"

// Ventana principal del sistema. Contiene un QStackedWidget que
// alterna entre la pantalla de inicio, el panel de administrador
// y el panel de cliente. Las 4 estructuras de datos viven aqui,
// como miembros de la ventana, para que todos los paneles trabajen
// sobre los mismos datos.
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    QStackedWidget* pilaPantallas;

    // Las 4 estructuras de datos del sistema, compartidas por todos los paneles.
    ArbolBinarioPeliculas cartelera;
    ListaCircularPromociones promociones;
    ListaCircularSolicitudes solicitudes;
    MatrizDispersaAsientos matrizAsientos;

    QWidget* crearPantallaInicio();
    QWidget* crearPlaceholder(const QString& texto);

    // Indices de cada pantalla dentro del QStackedWidget.
    enum Pantalla {
        INICIO = 0,
        ADMIN = 1,
        CLIENTE = 2
    };
};

#endif // MAIN_WINDOW_H