#include "Sede.h"

void to_json(nlohmann::json& j, const Sede& s) {
    j = nlohmann::json{
        {"codigo", s.codigo},
        {"nombre", s.nombre},
        {"direccion", s.direccion},
        {"cantidad_salas", s.cantidadSalas},
        {"telefono", s.telefono}
    };
}

void from_json(const nlohmann::json& j, Sede& s) {
    j.at("codigo").get_to(s.codigo);
    j.at("nombre").get_to(s.nombre);
    j.at("direccion").get_to(s.direccion);
    j.at("cantidad_salas").get_to(s.cantidadSalas);
    j.at("telefono").get_to(s.telefono);
}
