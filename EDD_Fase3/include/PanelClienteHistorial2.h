#ifndef PANEL_CLIENTE_HISTORIAL2_H
#define PANEL_CLIENTE_HISTORIAL2_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <string>
#include "Blockchain.h"
#include "ArbolAVLFunciones.h"

// "Mis Compras" del cliente (enunciado Fase 3, "Funciones de Cliente en
// Taquilla", puntos 3 y 4):
//   - Historial de compras: se obtiene recorriendo el DATA de los
//     bloques de la BLOCKCHAIN y filtrando por id_cliente (no depende
//     de la Tabla Hash en memoria, por eso persiste entre ejecuciones).
//     Tambien se listan, marcadas como pendientes, las compras que
//     todavia no entraron a ningun bloque.
//   - Comprobante: detalle completo de una compra por su
//     codigo_transaccion, con el INDEX y el HASH del bloque que la
//     registra (comprobante verificable de que forma parte de la cadena).
class PanelClienteHistorial2 : public QWidget {
    Q_OBJECT

public:
    PanelClienteHistorial2(Blockchain& blockchainRef,
                            ArbolAVLFunciones& funcionesRef,
                            std::string idClienteRef,
                            QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onVerComprobante();
    void onFilaSeleccionada(int fila, int columna);

private:
    Blockchain& blockchain;
    ArbolAVLFunciones& funciones;
    std::string idCliente;

    QTableWidget* tabla;
    QLineEdit* campoCodigoTransaccion;
    QTextEdit* textoComprobante;

    void actualizarTabla();
};

#endif // PANEL_CLIENTE_HISTORIAL2_H
