#include "Funcion.h"

void to_json(nlohmann::json& j, const Funcion& f) {
    j = nlohmann::json{
        {"codigo_funcion", f.codigoFuncion},
        {"horario", f.horario},
        {"sala", f.sala},
        {"filas", f.filas},
        {"columnas", f.columnas},
        {"archivo_asientos", f.archivoAsientos},
        {"sede", f.codigoSede},
        {"fecha", f.fecha}
    };
}

void from_json(const nlohmann::json& j, Funcion& f) {
    j.at("codigo_funcion").get_to(f.codigoFuncion);
    j.at("horario").get_to(f.horario);
    j.at("sala").get_to(f.sala);
    j.at("filas").get_to(f.filas);
    j.at("columnas").get_to(f.columnas);
    f.archivoAsientos = j.value("archivo_asientos", std::string(""));
    // "sede" y "fecha" son nuevos en Fase 3; se leen con default vacio
    // para no romper la compatibilidad con JSON de Fase 2 sin estos campos.
    f.codigoSede = j.value("sede", std::string(""));
    f.fecha = j.value("fecha", std::string(""));
}

