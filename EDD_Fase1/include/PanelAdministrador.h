#ifndef PANEL_ADMINISTRADOR_H
#define PANEL_ADMINISTRADOR_H

#include <QWidget>
#include "ArbolBinarioPeliculas.h"
#include "ListaCircularPromociones.h"
#include "ListaCircularSolicitudes.h"
#include "MatrizDispersaAsientos.h"

// Panel del administrador: agrupa todas las funciones administrativas
// en pestanas (Peliculas, Promociones, Solicitudes, Funciones/Asientos).
// Recibe referencias a las 4 estructuras de datos del sistema.
class PanelAdministrador : public QWidget {
    Q_OBJECT

public:
    PanelAdministrador(ArbolBinarioPeliculas& cartelera,
                        ListaCircularPromociones& promociones,
                        ListaCircularSolicitudes& solicitudes,
                        MatrizDispersaAsientos& matrizAsientos,
                        QWidget* parent = nullptr);

signals:
    void solicitudVolver();
};

#endif // PANEL_ADMINISTRADOR_H