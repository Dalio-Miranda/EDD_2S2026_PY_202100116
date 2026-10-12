#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QCloseEvent>

#include "ArbolBinarioPeliculas.h"
#include "ListaCircularPromociones.h"
#include "ListaCircularSolicitudes.h"
#include "MatrizDispersaAsientos.h"
#include "ArbolAVLFunciones.h"
#include "ArbolBClientes.h"
#include "TablaHashReservas.h"
#include "GestorAsientosPorFuncion.h"
#include "GrafoSedes.h"
#include "Blockchain.h"

// Ventana principal del sistema. Contiene un QStackedWidget que
// alterna entre la pantalla de inicio, el panel de administrador
// y el panel de cliente. Las estructuras de datos viven aqui,
// como miembros de la ventana, para que todos los paneles trabajen
// sobre los mismos datos.
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

protected:
    // Fase 3: al cerrar la ventana se serializa el estado actual de
    // sedes, peliculas y funciones y se comprime con LZW a los .edd.
    void closeEvent(QCloseEvent* event) override;

private:
    QStackedWidget* pilaPantallas;

    // Estructuras de datos de Fase 1, compartidas por todos los paneles.
    ArbolBinarioPeliculas cartelera;
    ListaCircularPromociones promociones;
    ListaCircularSolicitudes solicitudes;
    MatrizDispersaAsientos matrizAsientos;

    // Estructuras nuevas de Fase 2.
    ArbolAVLFunciones arbolFunciones;
    ArbolBClientes arbolClientes;
    TablaHashReservas tablaReservas;
    GestorAsientosPorFuncion gestorAsientos;

    // Estructura nueva de Fase 3.
    GrafoSedes grafoSedes;
    Blockchain blockchain{"../blocks"}; // carpeta blocks/ en la raiz del proyecto (como reports/)

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