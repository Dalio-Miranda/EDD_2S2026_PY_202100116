#ifndef PANEL_CLIENTE_RESERVAR2_H
#define PANEL_CLIENTE_RESERVAR2_H

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QTableWidget>
#include <string>
#include "ArbolBinarioPeliculas.h"
#include "ArbolAVLFunciones.h"
#include "GestorAsientosPorFuncion.h"
#include "TablaHashReservas.h"
#include "ArbolBClientes.h"
#include "GrafoSedes.h"
#include "Blockchain.h"

// Flujo completo de compra de un boleto (enunciado Fase 3, "Funciones
// de Cliente en Taquilla"):
//   1. Elegir una pelicula de la cartelera (BST).
//   2. Ver en que SEDES esta disponible, con horario y sala de cada
//      funcion (consulta sobre los vertices del grafo / las funciones).
//   3. Elegir una sede y ver SOLO las funciones de esa pelicula ahi.
//   4. Opcionalmente ver las SEDES SIMILARES a la elegida (aristas de
//      mayor peso del grafo).
//   5. Elegir asiento en el mapa de la funcion y CONFIRMAR EL PAGO
//      (Tarjeta o Efectivo): se crea la reserva (matriz dispersa +
//      Tabla Hash) y la transaccion se agrega a la cola de pendientes
//      de la blockchain.
class PanelClienteReservar2 : public QWidget {
    Q_OBJECT

public:
    PanelClienteReservar2(ArbolBinarioPeliculas& carteleraRef,
                           ArbolAVLFunciones& funcionesRef,
                           GestorAsientosPorFuncion& gestorRef,
                           TablaHashReservas& reservasRef,
                           ArbolBClientes& clientesRef,
                           GrafoSedes& grafoRef,
                           Blockchain& blockchainRef,
                           std::string idClienteRef,
                           QWidget* parent = nullptr);

    // Precio fijo de un boleto, en quetzales (el enunciado no define
    // tarifas; se usa el monto del ejemplo de transaccion: 45.0).
    static constexpr double PRECIO_BOLETO = 45.0;

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onCambioPelicula();
    void onCambioSede();
    void onClickFilaFuncion(int fila, int columna);
    void onVerSedesSimilares();
    void onCargarFuncion();
    void onClickAsiento(int fila, int columna);

private:
    ArbolBinarioPeliculas& cartelera;
    ArbolAVLFunciones& funciones;
    GestorAsientosPorFuncion& gestor;
    TablaHashReservas& reservas;
    ArbolBClientes& clientes;
    GrafoSedes& grafo;
    Blockchain& blockchain;
    std::string idCliente;

    QComboBox* comboPelicula;
    QComboBox* comboSede;
    QComboBox* comboFuncion;
    QTableWidget* tablaFuncionesPelicula;
    QLabel* labelInfo;
    QWidget* contenedorGrilla;
    QGridLayout* layoutGrilla;

    void llenarPeliculas();
    void llenarSedesDe(const std::string& codigoPelicula);
    void llenarFuncionesDe(const std::string& codigoPelicula, const std::string& codigoSede);
    void llenarTablaFuncionesDePelicula(const std::string& codigoPelicula);
    void dibujarGrilla();
    void limpiarGrilla();
    std::string generarCodigoReservaUnico() const;
};

#endif // PANEL_CLIENTE_RESERVAR2_H
