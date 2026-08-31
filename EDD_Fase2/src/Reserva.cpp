#include "Reserva.h"

void to_json(nlohmann::json& j, const Reserva& r) {
    j = nlohmann::json{
        {"codigo_reserva", r.codigoReserva},
        {"codigo_funcion", r.codigoFuncion},
        {"id_cliente", r.idCliente},
        {"fila", r.fila},
        {"columna", r.columna},
        {"fecha_reserva", r.fechaReserva}
    };
}

void from_json(const nlohmann::json& j, Reserva& r) {
    j.at("codigo_reserva").get_to(r.codigoReserva);
    j.at("codigo_funcion").get_to(r.codigoFuncion);
    // id_cliente puede no venir en el JSON de asientos de la funcion
    // (F00X_funcion.json), que solo trae codigo_reserva/fila/columna.
    r.idCliente = j.value("id_cliente", std::string(""));
    j.at("fila").get_to(r.fila);
    j.at("columna").get_to(r.columna);
    r.fechaReserva = j.value("fecha_reserva", std::string(""));
}
