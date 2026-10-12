#ifndef PANEL_CLIENTE_H
#define PANEL_CLIENTE_H

#include <QWidget>
#include <QStackedWidget>
#include <QLineEdit>
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

class PanelCliente : public QWidget {
    Q_OBJECT

public:
    PanelCliente(ArbolBinarioPeliculas& cartelera,
                 ListaCircularPromociones& promociones,
                 ListaCircularSolicitudes& solicitudes,
                 MatrizDispersaAsientos& matrizAsientos,
                 ArbolAVLFunciones& arbolFunciones,
                 ArbolBClientes& arbolClientes,
                 TablaHashReservas& tablaReservas,
                 GestorAsientosPorFuncion& gestorAsientos,
                 GrafoSedes& grafoSedesRef,
                 Blockchain& blockchainRef,
                 QWidget* parent = nullptr);

signals:
    void solicitudVolver();

private slots:
    void onLoginClicked();
    void onCerrarSesion();

private:
    ArbolBinarioPeliculas& cartelera;
    ListaCircularPromociones& promociones;
    ListaCircularSolicitudes& solicitudes;
    MatrizDispersaAsientos& matrizAsientos;
    ArbolAVLFunciones& funciones;
    ArbolBClientes& clientes;
    TablaHashReservas& reservas;
    GestorAsientosPorFuncion& gestor;
    GrafoSedes& grafo;
    Blockchain& blockchain;

    QStackedWidget* pila;
    QWidget* paginaLogin;
    QWidget* paginaSesion;

    QLineEdit* campoLoginCorreo;
    QLineEdit* campoLoginPassword;

    std::string idClienteActual;

    QWidget* construirPaginaLogin();
    void construirPaginaSesion();

    enum Pagina { LOGIN = 0, SESION = 1 };
};

#endif // PANEL_CLIENTE_H
