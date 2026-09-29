#ifndef SEDE_H
#define SEDE_H

#include <string>
#include <nlohmann/json.hpp>

// Representa una sede del cine. Cada sede es un vertice del
// GrafoSedes (enunciado Fase 3, 4.1).
struct Sede {
    std::string codigo;
    std::string nombre;
    std::string direccion;
    int cantidadSalas = 0;
    std::string telefono;

    Sede() = default;

    Sede(std::string codigo,
         std::string nombre,
         std::string direccion,
         int cantidadSalas,
         std::string telefono)
        : codigo(std::move(codigo)),
          nombre(std::move(nombre)),
          direccion(std::move(direccion)),
          cantidadSalas(cantidadSalas),
          telefono(std::move(telefono)) {}
};

void to_json(nlohmann::json& j, const Sede& s);
void from_json(const nlohmann::json& j, Sede& s);

#endif // SEDE_H
