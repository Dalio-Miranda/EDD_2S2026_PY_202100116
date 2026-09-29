#ifndef PANEL_CLIENTE_PROMOCIONES_H
#define PANEL_CLIENTE_PROMOCIONES_H

#include <QWidget>
#include <QTableWidget>
#include "ListaCircularPromociones.h"

// Pestana de cliente para ver las promociones VIGENTES segun la
// fecha actual (fechaInicio <= hoy <= fechaFin), con sus beneficios.
class PanelClientePromociones : public QWidget {
    Q_OBJECT

public:
    explicit PanelClientePromociones(ListaCircularPromociones& promocionesRef, QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onSeleccionCambiada();

private:
    ListaCircularPromociones& promociones;
    QTableWidget* tablaPromociones;
    QTableWidget* tablaBeneficios;

    void actualizarTablas();
};

#endif // PANEL_CLIENTE_PROMOCIONES_H