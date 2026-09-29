#ifndef PANEL_CLIENTE_CANCELAR_H
#define PANEL_CLIENTE_CANCELAR_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include "MatrizDispersaAsientos.h"

// Pestana de cliente para cancelar una reserva existente:
// ingresa su nombre, ve todas sus reservas activas, selecciona
// una y la cancela (el asiento vuelve a LIBRE).
class PanelClienteCancelarReserva : public QWidget {
    Q_OBJECT

public:
    explicit PanelClienteCancelarReserva(MatrizDispersaAsientos& matrizRef, QWidget* parent = nullptr);

private slots:
    void onBuscarReservas();
    void onCancelarSeleccionada();

private:
    MatrizDispersaAsientos& matriz;
    QLineEdit* campoNombre;
    QTableWidget* tabla;
};

#endif // PANEL_CLIENTE_CANCELAR_H