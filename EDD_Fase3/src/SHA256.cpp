#include "SHA256.h"
#include "ArregloDinamico.h"
#include <cstdint>
#include <sstream>
#include <iomanip>

namespace {

// Rotacion circular a la derecha (operacion basica de SHA-256).
inline uint32_t rotr(uint32_t x, uint32_t n) {
    return (x >> n) | (x << (32 - n));
}

// Las 64 constantes de ronda: son los primeros 32 bits de la parte
// fraccionaria de las raices cubicas de los primeros 64 numeros
// primos. Son parte fija del estandar SHA-256 (FIPS 180-4), no se
// "calculan", se copian tal cual del estandar.
const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

} // namespace

std::string SHA256::calcular(const std::string& mensaje) {
    // ---- Valores iniciales de hash: primeros 32 bits de la parte
    // fraccionaria de las raices cuadradas de los primeros 8 primos.
    // Tambien son parte fija del estandar.
    uint32_t h0 = 0x6a09e667, h1 = 0xbb67ae85, h2 = 0x3c6ef372, h3 = 0xa54ff53a;
    uint32_t h4 = 0x510e527f, h5 = 0x9b05688c, h6 = 0x1f83d9ab, h7 = 0x5be0cd19;

    // ---- Paso 1: relleno (padding) del mensaje ----
    // Se agrega un bit '1' (byte 0x80), luego ceros hasta que la
    // longitud sea congruente con 56 modulo 64, y finalmente la
    // longitud ORIGINAL del mensaje en bits, como entero de 64 bits
    // big-endian. Esto garantiza que el mensaje final sea multiplo
    // exacto de 64 bytes (512 bits), el tamanio de bloque que
    // procesa el algoritmo.
    uint64_t longitudBits = static_cast<uint64_t>(mensaje.size()) * 8;

    ArregloDinamico<unsigned char> datos;
    for (size_t i = 0; i < mensaje.size(); i++) {
        datos.agregarFinal(static_cast<unsigned char>(mensaje[i]));
    }
    datos.agregarFinal(0x80);
    while (datos.getTamanio() % 64 != 56) {
        datos.agregarFinal(0x00);
    }
    for (int i = 7; i >= 0; i--) {
        datos.agregarFinal(static_cast<unsigned char>((longitudBits >> (i * 8)) & 0xFF));
    }

    // ---- Paso 2: procesar el mensaje en bloques de 64 bytes ----
    int totalBloques = datos.getTamanio() / 64;

    for (int bloque = 0; bloque < totalBloques; bloque++) {
        uint32_t w[64];

        // Las primeras 16 palabras se toman directo del bloque (big-endian).
        for (int i = 0; i < 16; i++) {
            int base = bloque * 64 + i * 4;
            w[i] = (static_cast<uint32_t>(datos[base]) << 24) |
                   (static_cast<uint32_t>(datos[base + 1]) << 16) |
                   (static_cast<uint32_t>(datos[base + 2]) << 8) |
                   (static_cast<uint32_t>(datos[base + 3]));
        }

        // Las 48 palabras restantes se extienden a partir de las anteriores.
        for (int i = 16; i < 64; i++) {
            uint32_t s0 = rotr(w[i - 15], 7) ^ rotr(w[i - 15], 18) ^ (w[i - 15] >> 3);
            uint32_t s1 = rotr(w[i - 2], 17) ^ rotr(w[i - 2], 19) ^ (w[i - 2] >> 10);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }

        uint32_t a = h0, b = h1, c = h2, d = h3;
        uint32_t e = h4, f = h5, g = h6, h = h7;

        // Las 64 rondas de compresion.
        for (int i = 0; i < 64; i++) {
            uint32_t s1 = rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25);
            uint32_t ch = (e & f) ^ ((~e) & g);
            uint32_t temp1 = h + s1 + ch + K[i] + w[i];
            uint32_t s0 = rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22);
            uint32_t maj = (a & b) ^ (a & c) ^ (b & c);
            uint32_t temp2 = s0 + maj;

            h = g;
            g = f;
            f = e;
            e = d + temp1;
            d = c;
            c = b;
            b = a;
            a = temp1 + temp2;
        }

        h0 += a; h1 += b; h2 += c; h3 += d;
        h4 += e; h5 += f; h6 += g; h7 += h;
    }

    // ---- Paso 3: concatenar los 8 valores finales en hexadecimal ----
    std::ostringstream salida;
    salida << std::hex << std::setfill('0');
    for (uint32_t valor : {h0, h1, h2, h3, h4, h5, h6, h7}) {
        salida << std::setw(8) << valor;
    }
    return salida.str();
}
