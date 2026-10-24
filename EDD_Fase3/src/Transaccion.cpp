#include "Transaccion.h"
#include <sstream>

std::string Transaccion::aTexto() const {
    std::ostringstream oss;
    oss << codigoTransaccion << codigoReserva << idCliente << codigoFuncion
        << fila << columna << monto << metodoPago << fechaPago;
    // El estado solo entra al texto cuando es CANCELADA, asi el hash de
    // las compras normales no cambia respecto a bloques ya existentes.
    if (estado == "CANCELADA") oss << estado;
    return oss.str();
}

void to_json(nlohmann::json& j, const Transaccion& t) {
    j = nlohmann::json{
        {"codigo_transaccion", t.codigoTransaccion},
        {"codigo_reserva", t.codigoReserva},
        {"id_cliente", t.idCliente},
        {"codigo_funcion", t.codigoFuncion},
        {"fila", t.fila},
        {"columna", t.columna},
        {"monto", t.monto},
        {"metodo_pago", t.metodoPago},
        {"fecha_pago", t.fechaPago},
        {"estado", t.estado}
    };
}

void from_json(const nlohmann::json& j, Transaccion& t) {
    j.at("codigo_transaccion").get_to(t.codigoTransaccion);
    j.at("codigo_reserva").get_to(t.codigoReserva);
    j.at("id_cliente").get_to(t.idCliente);
    j.at("codigo_funcion").get_to(t.codigoFuncion);
    j.at("fila").get_to(t.fila);
    j.at("columna").get_to(t.columna);
    j.at("monto").get_to(t.monto);
    j.at("metodo_pago").get_to(t.metodoPago);
    j.at("fecha_pago").get_to(t.fechaPago);
    // Opcional: los JSON anteriores no traen "estado" (= compra PAGADA).
    t.estado = j.value("estado", std::string("PAGADA"));
}
