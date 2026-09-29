#ifndef SHA256_H
#define SHA256_H

#include <string>

// Implementacion manual del algoritmo de hash SHA-256 (FIPS 180-4),
// sin usar ninguna libreria de criptografia externa. Se usa para
// calcular el HASH de cada bloque de la blockchain (enunciado Fase 3,
// seccion "Seguridad") y los hashes de las hojas/nodos internos del
// Arbol de Merkle.
class SHA256 {
public:
    // Calcula el hash SHA-256 del texto dado y lo devuelve como
    // string hexadecimal en minusculas de 64 caracteres (256 bits).
    static std::string calcular(const std::string& mensaje);
};

#endif // SHA256_H
