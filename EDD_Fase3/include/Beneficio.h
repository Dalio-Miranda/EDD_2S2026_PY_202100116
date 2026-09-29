#ifndef BENEFICIO_H
#define BENEFICIO_H

#include <string>

// Un beneficio asociado a una promoción (ej: "2x1 en entradas").
struct Beneficio {
    std::string tipo;        // "descuento", "combo", "2x1"
    std::string descripcion; // "Por la compra de 1 boleto llevate el otro gratis"
    std::string valor;       // "5%", "10%", "NA" (no aplica)
};

#endif // BENEFICIO_H
