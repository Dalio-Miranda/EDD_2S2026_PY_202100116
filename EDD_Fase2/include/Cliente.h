#ifndef CLIENTE_H
#define CLIENTE_H

#include <string>
#include <vector>
#include <nlohmann/json.hpp>

// Representa un cliente del sistema. Se indexa en el ArbolBClientes
// usando "id" como clave.
//
// Segun el enunciado (4.1, punto 04 y 05), el cliente mantiene
// UNICAMENTE las referencias (codigo_reserva) de sus reservas, no el
// objeto completo -- el detalle de cada reserva vive en la Tabla Hash
// general de reservas, para evitar duplicar informacion.
struct Cliente {
    std::string id;
    std::string nombre;
    std::string correo;
    std::string telefono;
    std::string password;
    std::string tipo; // "cliente" (el enunciado no define otros tipos aqui)

    // Solo los codigos, no los objetos Reserva completos.
    std::vector<std::string> codigosReserva;

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

// Serializacion propia del Cliente tal como vive en el Arbol B (solo
// referencias de reserva). La carga masiva del JSON de "clientes con
// reservas anidadas" (enunciado 4.1 punto 04) es un formato de
// ENTRADA distinto -- ese parseo especial vive en PersistenciaJSON,
// que separa cada reserva completa hacia la Tabla Hash y deja aqui
// solo el codigo_reserva.
void to_json(nlohmann::json& j, const Cliente& c);
void from_json(const nlohmann::json& j, Cliente& c);

#endif // CLIENTE_H
