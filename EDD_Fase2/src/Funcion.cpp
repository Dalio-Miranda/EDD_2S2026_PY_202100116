#include "Funcion.h"

void to_json(nlohmann::json& j, const Funcion& f) {
    j = nlohmann::json{
        {"codigo_funcion", f.codigoFuncion},
        {"horario", f.horario},
        {"sala", f.sala},
        {"filas", f.filas},
        {"columnas", f.columnas},
        {"archivo_asientos", f.archivoAsientos}
    };
}

void from_json(const nlohmann::json& j, Funcion& f) {
    j.at("codigo_funcion").get_to(f.codigoFuncion);
    j.at("horario").get_to(f.horario);
    j.at("sala").get_to(f.sala);
    j.at("filas").get_to(f.filas);
    j.at("columnas").get_to(f.columnas);

    // archivo_asientos es opcional en el JSON de carga masiva (inicia
    // vacio segun el enunciado), por eso no usamos at() aqui.
    f.archivoAsientos = j.value("archivo_asientos", std::string(""));
}
