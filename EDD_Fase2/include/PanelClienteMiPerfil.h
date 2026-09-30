#ifndef PANEL_CLIENTE_MI_PERFIL_H
#define PANEL_CLIENTE_MI_PERFIL_H

#include <QWidget>
#include <QLineEdit>
#include <string>
#include "ArbolBClientes.h"

// Pestana de cliente para editar su propio perfil (enunciado 4.1,
// "Funciones de Cliente en Taquilla", punto 01: "El usuario tendra un
// panel para editar su perfil"). El ID del cliente no es editable (es
// la clave del Arbol B); nombre, correo, telefono y password si.
class PanelClienteMiPerfil : public QWidget {
    Q_OBJECT

public:
    PanelClienteMiPerfil(ArbolBClientes& clientesRef,
                          std::string idClienteRef,
                          QWidget* parent = nullptr);

protected:
    void showEvent(QShowEvent* event) override;

private slots:
    void onGuardarCambios();

private:
    ArbolBClientes& clientes;
    std::string idCliente;

    QLineEdit* campoNombre;
    QLineEdit* campoCorreo;
    QLineEdit* campoTelefono;
    QLineEdit* campoPasswordNueva;

    void cargarDatosActuales();
};

#endif // PANEL_CLIENTE_MI_PERFIL_H
