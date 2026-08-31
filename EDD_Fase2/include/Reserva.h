#ifndef RESERVA_H
#define RESERVA_H

#include <string>
#include <nlohmann/json.hpp>

// Representa una reserva de un asiento. Se indexa en la Tabla Hash
// general usando codigoReserva como clave.
//
// Segun el enunciado (4.1, punto 04), la reserva mantiene solo las
// referencias necesarias para relacionarse con las demas estructuras
// -- principalmente codigoFuncion -- en vez de duplicar datos de la
// pelicula o del cliente.
struct Reserva {
    std::string codigoReserva;
    std::string codigoFuncion;
    std::string idCliente; // referencia al cliente dueno de la reserva
    int fila = 0;
    int columna = 0;
    std::string fechaReserva; // formato "YYYY-MM-DD"

    Reserva() = default;

    Reserva(std::string codigoReserva,
            std::string codigoFuncion,
            std::string idCliente,
            int fila,
            int columna,
            std::string fechaReserva)
        : codigoReserva(std::move(codigoReserva)),
          codigoFuncion(std::move(codigoFuncion)),
          idCliente(std::move(idCliente)),
          fila(fila),
          columna(columna),
          fechaReserva(std::move(fechaReserva)) {}
};

void to_json(nlohmann::json& j, const Reserva& r);
void from_json(const nlohmann::json& j, Reserva& r);

#endif // RESERVA_H
