#ifndef RESERVA_H
#define RESERVA_H

#include <string>
#include <nlohmann/json.hpp>

struct Reserva {
    std::string codigoReserva;
    std::string codigoFuncion;
    std::string idCliente;
    int fila = 0;
    int columna = 0;
    std::string fechaReserva;

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
