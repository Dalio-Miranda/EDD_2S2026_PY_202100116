#ifndef PANEL_CLIENTE_ASIENTOS_H
#define PANEL_CLIENTE_ASIENTOS_H

#include <QWidget>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include "MatrizDispersaAsientos.h"

// Pestana de cliente para reservar asientos de la funcion activa.
// Muestra la misma grilla verde/rojo; clic en un asiento libre
// reserva a nombre del cliente ingresado arriba, clic en uno
// ocupado avisa que esta tomado.
class PanelClienteAsientos : public QWidget {
    Q_OBJECT

public:
    explicit PanelClienteAsientos(MatrizDispersaAsientos& matrizRef, QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onClickAsiento(int fila, int columna);
    void onActualizar();

private:
    MatrizDispersaAsientos& matriz;
    QLineEdit* campoNombre;
    QLabel* labelInfo;
    QWidget* contenedorGrilla;
    QGridLayout* layoutGrilla;

    void dibujarGrilla();
    void limpiarGrilla();
};

#endif // PANEL_CLIENTE_ASIENTOS_H