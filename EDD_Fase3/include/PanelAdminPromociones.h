#ifndef PANEL_ADMIN_PROMOCIONES_H
#define PANEL_ADMIN_PROMOCIONES_H

#include <QWidget>
#include <QTableWidget>
#include "ListaCircularPromociones.h"

// Pestana del administrador para gestionar promociones y sus
// beneficios asociados. Tabla de promociones arriba; al seleccionar
// una fila, la tabla de beneficios de abajo se actualiza mostrando
// los beneficios de esa promocion en particular.
class PanelAdminPromociones : public QWidget {
    Q_OBJECT

public:
    explicit PanelAdminPromociones(ListaCircularPromociones& promocionesRef, QWidget* parent = nullptr);

private slots:
    void onAgregarPromocion();
    void onAnadirBeneficio();
    void onEliminarPromocion();
    void onGenerarReporte();
    void onSeleccionCambiada();

private:
    ListaCircularPromociones& promociones;
    QTableWidget* tablaPromociones;
    QTableWidget* tablaBeneficios;

    void actualizarTablaPromociones();
    void actualizarTablaBeneficios(const QString& codigoPromocion);
    QString codigoSeleccionado() const;
};

#endif // PANEL_ADMIN_PROMOCIONES_H