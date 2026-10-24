#ifndef TRANSACCION_H
#define TRANSACCION_H

#include <string>
#include <nlohmann/json.hpp>

// Una transaccion = el pago de UNA reservacion. Vive dentro del
// campo DATA de un Bloque (enunciado Fase 3, "Blockchain"). Contiene
// toda la informacion necesaria para, a partir unicamente de la
// blockchain, poder reconstruir la Tabla Hash de reservas.
struct Transaccion {
    std::string codigoTransaccion;
    std::string codigoReserva;
    std::string idCliente;
    std::string codigoFuncion;
    int fila = 0;
    int columna = 0;
    double monto = 0.0;
    std::string metodoPago; // "Tarjeta" o "Efectivo"
    std::string fechaPago;  // formato DD-MM-AAAA
    // "PAGADA" (compra normal) o "CANCELADA". Una cancelacion NO modifica
    // el bloque donde quedo la compra (los bloques son inmutables): se
    // registra como una transaccion NUEVA, con el mismo codigo de
    // transaccion y estado CANCELADA, que viaja en un bloque posterior.
    std::string estado = "PAGADA";

    Transaccion() = default;

    Transaccion(std::string codigoTransaccion,
                std::string codigoReserva,
                std::string idCliente,
                std::string codigoFuncion,
                int fila,
                int columna,
                double monto,
                std::string metodoPago,
                std::string fechaPago)
        : codigoTransaccion(std::move(codigoTransaccion)),
          codigoReserva(std::move(codigoReserva)),
          idCliente(std::move(idCliente)),
          codigoFuncion(std::move(codigoFuncion)),
          fila(fila),
          columna(columna),
          monto(monto),
          metodoPago(std::move(metodoPago)),
          fechaPago(std::move(fechaPago)) {}

    // Representacion canonica en texto de la transaccion (todos sus
    // campos concatenados en un orden fijo). Es la entrada que se le
    // da a SHA256 para calcular el hash de la hoja del Arbol de
    // Merkle correspondiente a esta transaccion.
    std::string aTexto() const;
};

void to_json(nlohmann::json& j, const Transaccion& t);
void from_json(const nlohmann::json& j, Transaccion& t);

#endif // TRANSACCION_H
