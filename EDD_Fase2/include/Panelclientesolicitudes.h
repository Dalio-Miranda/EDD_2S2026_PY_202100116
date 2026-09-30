#ifndef PANEL_CLIENTE_SOLICITUDES_H
#define PANEL_CLIENTE_SOLICITUDES_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include "ListaCircularSolicitudes.h"

// Pestana de cliente para crear una solicitud especial y consultar
// el estado de sus solicitudes: por defecto muestra TODAS las
// registradas, con opcion de filtrar por numero de telefono.
class PanelClienteSolicitudes : public QWidget {
    Q_OBJECT

public:
    explicit PanelClienteSolicitudes(ListaCircularSolicitudes& solicitudesRef, QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onEnviarSolicitud();
    void onConsultarEstado();
    void onVerTodas();

private:
    ListaCircularSolicitudes& solicitudes;

    QLineEdit* campoNombre;
    QLineEdit* campoTelefono;
    QComboBox* campoTipo;
    QTextEdit* campoDescripcion;

    QLineEdit* campoTelefonoConsulta;
    QTableWidget* tablaResultados;

    void mostrarResultados(const ArregloDinamico<Solicitud>& resultados);
};

#endif // PANEL_CLIENTE_SOLICITUDES_H