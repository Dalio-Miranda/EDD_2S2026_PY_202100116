#ifndef BLOQUE_H
#define BLOQUE_H

#include <string>
#include "ArregloDinamico.h"
#include "Transaccion.h"
#include <nlohmann/json.hpp>

// Cuantos ceros al inicio debe tener el HASH para que un bloque sea
// valido (prueba de trabajo). El enunciado fija este valor en 4.
constexpr int DIFICULTAD_PRUEBA_TRABAJO = 4;

// Un bloque de la blockchain (enunciado Fase 3, seccion "Bloque").
// HASH = SHA256(INDEX + TIMESTAMP + NONCE + ROOTMERKLE + PREVIOUSHASH),
// concatenados sin espacios. El campo NONCE se encuentra iterando
// desde 0 hasta que el HASH resultante empiece con
// DIFICULTAD_PRUEBA_TRABAJO ceros (prueba de trabajo).
struct Bloque {
    int index = 0;
    std::string timestamp;  // formato DD-MM-AAAA::HH:MM:SS
    long long nonce = 0;
    ArregloDinamico<Transaccion> data;
    std::string rootMerkle;
    std::string previousHash;
    std::string hash;

    Bloque() = default;

    // Calcula el hash del bloque a partir de sus campos ACTUALES
    // (index, timestamp, nonce, rootMerkle, previousHash) -- sin usar
    // el campo "hash" ya almacenado. Se usa tanto durante la minado
    // (prueba de trabajo) como para verificar la integridad de un
    // bloque leido desde disco.
    std::string calcularHash() const;

    // Prueba de trabajo: itera "nonce" desde 0 hasta encontrar un
    // hash que empiece con DIFICULTAD_PRUEBA_TRABAJO ceros, y lo dejo
    // guardado en el campo "hash". Debe llamarse despues de fijar
    // index, timestamp, data, rootMerkle y previousHash.
    void minar();

    // true si el hash almacenado coincide con el que se obtiene
    // recalculandolo desde los campos actuales del bloque (osea, que
    // el bloque no fue modificado despues de minarse).
    bool esValido() const;

    // Genera el timestamp actual en el formato DD-MM-AAAA::HH:MM:SS
    // que pide el enunciado.
    static std::string timestampActual();
};

void to_json(nlohmann::json& j, const Bloque& b);
void from_json(const nlohmann::json& j, Bloque& b);

#endif // BLOQUE_H
