#ifndef PANEL_CLIENTE_H
#define PANEL_CLIENTE_H

#include <QWidget>
#include "ArbolBinarioPeliculas.h"
#include "ListaCircularPromociones.h"
#include "ListaCircularSolicitudes.h"
#include "MatrizDispersaAsientos.h"

// Panel del cliente: agrupa todas las funciones de cliente en
// pestanas (Cartelera, Reservar Asientos, Cancelar Reserva,
// Promociones, Solicitudes).
class PanelCliente : public QWidget {
    Q_OBJECT

public:
    PanelCliente(ArbolBinarioPeliculas& cartelera,
                 ListaCircularPromociones& promociones,
                 ListaCircularSolicitudes& solicitudes,
                 MatrizDispersaAsientos& matrizAsientos,
                 QWidget* parent = nullptr);

signals:
    void solicitudVolver();
};

#endif // PANEL_CLIENTE_H