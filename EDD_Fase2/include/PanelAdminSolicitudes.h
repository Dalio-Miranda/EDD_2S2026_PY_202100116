#ifndef PANEL_ADMIN_SOLICITUDES_H
#define PANEL_ADMIN_SOLICITUDES_H

#include <QWidget>
#include <QTableWidget>
#include <QLabel>
#include "ListaCircularSolicitudes.h"

// Pestana del administrador para gestionar las solicitudes especiales
// de los clientes: ver todas, aprobar (cambiar estado) o rechazar
// (eliminar de la lista), y ver el total de pendientes.
class PanelAdminSolicitudes : public QWidget {
    Q_OBJECT

public:
    explicit PanelAdminSolicitudes(ListaCircularSolicitudes& solicitudesRef, QWidget* parent = nullptr);

protected:
    // Se dispara cada vez que el widget se vuelve visible, incluyendo
    // cuando el usuario cambia a esta pestana dentro del QTabWidget.
    // Sin esto, la tabla solo se llenaba una vez al arrancar el programa
    // (cuando todavia no existian solicitudes de clientes) y nunca se
    // refrescaba, aunque los datos si se guardaban bien en la lista.
    void showEvent(QShowEvent* evento) override;

private slots:
    void onAprobar();
    void onRechazar();
    void onGenerarReporte();

private:
    ListaCircularSolicitudes& solicitudes;
    QTableWidget* tabla;
    QLabel* labelContador;

    void actualizarTabla();
    int numeroSeleccionado() const; // retorna -1 si no hay seleccion
};

#endif // PANEL_ADMIN_SOLICITUDES_H