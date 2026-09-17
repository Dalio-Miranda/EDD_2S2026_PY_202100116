#include "Cliente.h"

void to_json(nlohmann::json& j, const Cliente& c) {
    nlohmann::json codigos = nlohmann::json::array();
    for (const std::string& codigo : c.codigosReserva) {
        codigos.push_back(codigo);
    }

    j = nlohmann::json{
        {"id", c.id},
        {"nombre", c.nombre},
        {"correo", c.correo},
        {"telefono", c.telefono},
        {"password", c.password},
        {"tipo", c.tipo},
        {"codigos_reserva", codigos}
    };
}

void from_json(const nlohmann::json& j, Cliente& c) {
    j.at("id").get_to(c.id);
    j.at("nombre").get_to(c.nombre);
    j.at("correo").get_to(c.correo);
    j.at("telefono").get_to(c.telefono);
    j.at("password").get_to(c.password);
    c.tipo = j.value("tipo", std::string("cliente"));

    c.codigosReserva = ArregloDinamico<std::string>();
    if (j.contains("codigos_reserva")) {
        for (const auto& item : j.at("codigos_reserva")) {
            c.codigosReserva.agregarFinal(item.get<std::string>());
        }
    }
}
