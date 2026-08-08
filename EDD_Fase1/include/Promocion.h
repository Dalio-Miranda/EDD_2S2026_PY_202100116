#ifndef PROMOCION_H
#define PROMOCION_H

#include <string>
#include <vector>
#include "ListaDobleBeneficios.h"

// Representa una promoción especial (ej: "Martes de Locura").
// Cada promoción "carga" su propia lista doblemente enlazada
// de beneficios asociados.
struct Promocion {
    std::string codigo;               // "Promo_01"
    std::string nombre;               // "Martes de Locura"
    std::string fechaInicio;          // AAAA-MM-DD
    std::string fechaFin;             // AAAA-MM-DD
    std::vector<std::string> diasAplicables; // {"Lunes", "Martes"}
    ListaDobleBeneficios beneficios;
};

#endif // PROMOCION_H
