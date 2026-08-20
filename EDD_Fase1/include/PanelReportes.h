#ifndef PANEL_REPORTES_H
#define PANEL_REPORTES_H

#include <QWidget>
#include <QLabel>
#include "ArbolBinarioPeliculas.h"
#include "ListaCircularSolicitudes.h"
#include "ListaCircularPromociones.h"
#include "MatrizDispersaAsientos.h"

// Pestana que muestra los 4 reportes de Graphviz DIRECTAMENTE
// dentro de la interfaz (no abre un visor externo). Tiene botones
// de navegacion (Anterior/Siguiente) para pasar entre los 4
// reportes tipo carrusel, y un boton para (re)generar el actual.
class PanelReportes : public QWidget {
    Q_OBJECT

public:
    PanelReportes(ArbolBinarioPeliculas& carteleraRef,
                  ListaCircularSolicitudes& solicitudesRef,
                  ListaCircularPromociones& promocionesRef,
                  MatrizDispersaAsientos& matrizRef,
                  QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onAnterior();
    void onSiguiente();
    void onGenerar();

private:
    ArbolBinarioPeliculas& cartelera;
    ListaCircularSolicitudes& solicitudes;
    ListaCircularPromociones& promociones;
    MatrizDispersaAsientos& matriz;

    int indiceActual;
    QLabel* labelTitulo;
    QLabel* labelImagen;

    void generarActual();
    void mostrarActual();
};

#endif // PANEL_REPORTES_H