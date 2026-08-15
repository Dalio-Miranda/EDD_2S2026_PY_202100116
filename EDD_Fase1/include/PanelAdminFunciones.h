#ifndef PANEL_ADMIN_FUNCIONES_H
#define PANEL_ADMIN_FUNCIONES_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include "MatrizDispersaAsientos.h"
#include "ArbolBinarioPeliculas.h"

// Pestana del administrador para crear la funcion (pelicula + horario +
// sala + dimensiones) y visualizar/gestionar el mapa de asientos como
// una grilla de botones (verde = libre, rojo = ocupado).
// Segun el enunciado, en Fase 1 solo existe una funcion a la vez:
// crear una nueva sobreescribe la anterior.
class PanelAdminFunciones : public QWidget {
    Q_OBJECT

public:
    PanelAdminFunciones(MatrizDispersaAsientos& matrizRef, ArbolBinarioPeliculas& carteleraRef, QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onCrearFuncion();
    void onGenerarReporte();
    void onClickAsiento(int fila, int columna);

private:
    MatrizDispersaAsientos& matriz;
    ArbolBinarioPeliculas& cartelera;

    QLabel* labelInfo;
    QWidget* contenedorGrilla;
    QGridLayout* layoutGrilla;

    void dibujarGrilla();
    void limpiarGrilla();
};

#endif // PANEL_ADMIN_FUNCIONES_H