#include "Cliente.h"

void to_json(nlohmann::json& j, const Cliente& c) {
    j = nlohmann::json{
        {"id", c.id},
        {"nombre", c.nombre},
        {"correo", c.correo},
        {"telefono", c.telefono},
        {"password", c.password},
        {"tipo", c.tipo},
        {"codigos_reserva", c.codigosReserva}
    };
}

void from_json(const nlohmann::json& j, Cliente& c) {
    j.at("id").get_to(c.id);
    j.at("nombre").get_to(c.nombre);
    j.at("correo").get_to(c.correo);
    j.at("telefono").get_to(c.telefono);
    j.at("password").get_to(c.password);
    c.tipo = j.value("tipo", std::string("cliente"));
    c.codigosReserva = j.value("codigos_reserva", std::vector<std::string>{});
}
