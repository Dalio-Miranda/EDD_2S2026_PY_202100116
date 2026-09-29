#ifndef CLIENTE_H
#define CLIENTE_H

#include <string>
#include <nlohmann/json.hpp>
#include "ArregloDinamico.h"

// Representa un cliente del sistema. Se indexa en el ArbolBClientes
// usando "id" como clave.
//
// codigosReserva guarda SOLO las referencias (codigo_reserva) de sus
// reservas, no los objetos completos -- el detalle de cada reserva
// vive en la Tabla Hash general, para evitar duplicar informacion
// (enunciado 4.1, puntos 04 y 05).
struct Cliente {
    std::string id;
    std::string nombre;
    std::string correo;
    std::string telefono;
    std::string password;
    std::string tipo; // "cliente"

    ArregloDinamico<std::string> codigosReserva;

    Cliente() = default;

    Cliente(std::string id,
            std::string nombre,
            std::string correo,
            std::string telefono,
            std::string password,
            std::string tipo = "cliente")
        : id(std::move(id)),
          nombre(std::move(nombre)),
          correo(std::move(correo)),
          telefono(std::move(telefono)),
          password(std::move(password)),
          tipo(std::move(tipo)) {}
};

void to_json(nlohmann::json& j, const Cliente& c);
void from_json(const nlohmann::json& j, Cliente& c);

#endif // CLIENTE_H
