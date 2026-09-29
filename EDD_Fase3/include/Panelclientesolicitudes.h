#ifndef PANEL_CLIENTE_SOLICITUDES_H
#define PANEL_CLIENTE_SOLICITUDES_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include "ListaCircularSolicitudes.h"

// Pestana de cliente para crear una solicitud especial y consultar
// el estado de sus solicitudes previas por numero de telefono.
class PanelClienteSolicitudes : public QWidget {
    Q_OBJECT

public:
    explicit PanelClienteSolicitudes(ListaCircularSolicitudes& solicitudesRef, QWidget* parent = nullptr);

private slots:
    void onEnviarSolicitud();
    void onConsultarEstado();

private:
    ListaCircularSolicitudes& solicitudes;

    QLineEdit* campoNombre;
    QLineEdit* campoTelefono;
    QComboBox* campoTipo;
    QTextEdit* campoDescripcion;

    QLineEdit* campoTelefonoConsulta;
    QTableWidget* tablaResultados;
};

#endif // PANEL_CLIENTE_SOLICITUDES_H