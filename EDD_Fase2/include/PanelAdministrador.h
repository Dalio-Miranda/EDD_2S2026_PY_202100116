#ifndef PANEL_ADMINISTRADOR_H
#define PANEL_ADMINISTRADOR_H

#include <QWidget>
#include "ArbolBinarioPeliculas.h"
#include "ListaCircularPromociones.h"
#include "ListaCircularSolicitudes.h"
#include "MatrizDispersaAsientos.h"
#include "ArbolAVLFunciones.h"
#include "ArbolBClientes.h"
#include "TablaHashReservas.h"
#include "GestorAsientosPorFuncion.h"

// Panel del administrador: agrupa todas las funciones administrativas
// en pestanas (Peliculas, Promociones, Solicitudes, Funciones/Asientos
// de Fase 1, mas las nuevas de Fase 2: Funciones (AVL), Clientes).
// Recibe referencias a todas las estructuras de datos del sistema.
class PanelAdministrador : public QWidget {
    Q_OBJECT

public:
    PanelAdministrador(ArbolBinarioPeliculas& cartelera,
                        ListaCircularPromociones& promociones,
                        ListaCircularSolicitudes& solicitudes,
                        MatrizDispersaAsientos& matrizAsientos,
                        ArbolAVLFunciones& arbolFunciones,
                        ArbolBClientes& arbolClientes,
                        TablaHashReservas& tablaReservas,
                        GestorAsientosPorFuncion& gestorAsientos,
                        QWidget* parent = nullptr);

signals:
    void solicitudVolver();
};

#endif // PANEL_ADMINISTRADOR_H
