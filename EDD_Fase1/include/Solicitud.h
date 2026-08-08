#ifndef SOLICITUD_H
#define SOLICITUD_H

#include <string>

// Representa una solicitud especial hecha por un cliente
// (cumpleaños, aniversario, requerimiento especial, queja, sugerencia).
struct Solicitud {
    int numero = 0;             // autogenerado al registrar
    std::string nombreCliente;
    std::string telefono;
    std::string tipoSolicitud;  // "cumpleanos", "aniversario", "requerimiento especial", "queja", "sugerencia"
    std::string descripcion;
    std::string fechaSolicitud; // se genera automaticamente
    std::string estado;         // "Pendiente", "En proceso", "Atendida", "Rechazada"
};

#endif // SOLICITUD_H