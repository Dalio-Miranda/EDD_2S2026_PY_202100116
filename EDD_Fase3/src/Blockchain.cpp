#include "Blockchain.h"
#include "ArbolMerkle.h"
#include "Reserva.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

Blockchain::Blockchain(std::string carpetaBlocks)
    : carpetaBlocks(std::move(carpetaBlocks)) {
    std::filesystem::create_directories(this->carpetaBlocks);
}

Blockchain::~Blockchain() {
    liberarCadena();
}

void Blockchain::liberarCadena() {
    while (cabeza != nullptr) {
        NodoBloque* siguiente = cabeza->siguiente;
        delete cabeza;
        cabeza = siguiente;
    }
    totalBloques = 0;
}

void Blockchain::insertarAlFrente(const Bloque& b) {
    NodoBloque* nuevo = new NodoBloque();
    nuevo->dato = b;
    nuevo->siguiente = cabeza;
    cabeza = nuevo;
    totalBloques++;
}

void Blockchain::persistirBloque(const Bloque& b) const {
    std::filesystem::create_directories(carpetaBlocks);
    std::string ruta = carpetaBlocks + "/block" + std::to_string(b.index) + ".json";
    std::ofstream archivo(ruta);
    nlohmann::json j = b;
    archivo << j.dump(2);
}

Blockchain::ResultadoCarga Blockchain::cargarDesdeDisco() {
    ResultadoCarga resultado;

    liberarCadena();

    // Leer todos los archivos block0.json, block1.json, ... en
    // orden, hasta que falte uno (o este corrupto).
    ArregloDinamico<Bloque> bloquesEnDisco;
    int indice = 0;
    while (true) {
        std::string ruta = carpetaBlocks + "/block" + std::to_string(indice) + ".json";
        std::ifstream archivo(ruta);
        if (!archivo.is_open()) break;

        try {
            nlohmann::json j;
            archivo >> j;
            bloquesEnDisco.agregarFinal(j.get<Bloque>());
        } catch (const std::exception&) {
            break; // archivo corrupto / no es JSON valido: se detiene aqui
        }
        indice++;
    }

    // Verificar la integridad de la cadena, en orden, desde el
    // genesis. Un bloque es valido si:
    //   1. su PREVIOUSHASH coincide con el hash real del bloque
    //      anterior (o "0000" si es el genesis),
    //   2. su HASH almacenado coincide con el recalculado a partir
    //      de sus propios campos (encabezado no alterado), y
    //   3. su ROOTMERKLE coincide con la raiz de Merkle recalculada
    //      a partir de su DATA actual (las transacciones no fueron
    //      alteradas).
    cadenaEnDisco.vaciar();
    cadenaEnDiscoIntegra.vaciar();

    int validos = 0;
    bool cadenaRota = false;
    for (int i = 0; i < bloquesEnDisco.getTamanio(); i++) {
        const Bloque& actual = bloquesEnDisco[i];

        bool prevOk = (i == 0)
            ? (actual.previousHash == "0000")
            : (actual.previousHash == bloquesEnDisco[i - 1].hash);

        bool hashOk = actual.esValido();

        std::string raizRecalculada = ArbolMerkle::calcularRaiz(actual.data);
        bool merkleOk = (raizRecalculada == actual.rootMerkle);

        // Estado para el reporte: integridad propia del bloque (hash y
        // Merkle) y que su PREVIOUSHASH apunte al bloque anterior leido.
        cadenaEnDisco.agregarFinal(actual);
        cadenaEnDiscoIntegra.agregarFinal(prevOk && hashOk && merkleOk);

        if (!cadenaRota && (!prevOk || !hashOk || !merkleOk)) {
            resultado.huboManipulacion = true;
            resultado.indiceDelPrimerManipulado = actual.index;
            cadenaRota = true;
        }
        if (!cadenaRota) validos++;
    }

    resultado.bloquesValidos = validos;
    resultado.bloquesDescartados = bloquesEnDisco.getTamanio() - validos;

    // Insertar SOLO los validos a la lista enlazada, del mas viejo al
    // mas nuevo (insertarAlFrente en ese orden deja la cabeza en el
    // bloque mas reciente, tal como debe quedar la cadena en memoria).
    for (int i = 0; i < validos; i++) {
        insertarAlFrente(bloquesEnDisco[i]);
    }

    return resultado;
}

void Blockchain::agregarTransaccionPendiente(const Transaccion& t) {
    pendientes.agregarFinal(t);
    if (pendientes.getTamanio() >= TRANSACCIONES_POR_BLOQUE_AUTOMATICO) {
        generarBloque();
    }
}

int Blockchain::getCantidadPendientes() const {
    return pendientes.getTamanio();
}

bool Blockchain::generarBloque() {
    if (pendientes.estaVacio()) return false;

    Bloque nuevo;
    nuevo.index = totalBloques;
    nuevo.timestamp = Bloque::timestampActual();
    nuevo.previousHash = hayBloques() ? cabeza->dato.hash : "0000";
    nuevo.data = pendientes;
    nuevo.rootMerkle = ArbolMerkle::calcularRaiz(nuevo.data);
    nuevo.minar();

    insertarAlFrente(nuevo);
    persistirBloque(nuevo);
    registrarEnCadenaDisco(nuevo, true);

    pendientes.vaciar();
    return true;
}

bool Blockchain::hayBloques() const {
    return cabeza != nullptr;
}

int Blockchain::getCantidadBloques() const {
    return totalBloques;
}

Bloque* Blockchain::getUltimoBloque() const {
    return cabeza == nullptr ? nullptr : &cabeza->dato;
}

NodoBloque* Blockchain::getCabeza() const {
    return cabeza;
}

void Blockchain::reconstruirTablaHash(TablaHashReservas& tabla) const {
    // La lista esta enlazada del mas nuevo al mas viejo; se
    // recolectan los punteros primero para poder recorrerlos en
    // reversa (genesis -> mas nuevo), que es el orden cronologico
    // real en el que las reservas deben insertarse.
    ArregloDinamico<Bloque*> enOrden;
    for (NodoBloque* n = cabeza; n != nullptr; n = n->siguiente) {
        enOrden.agregarFinal(&n->dato);
    }

    for (int i = enOrden.getTamanio() - 1; i >= 0; i--) {
        Bloque* b = enOrden[i];
        for (int j = 0; j < b->data.getTamanio(); j++) {
            const Transaccion& t = b->data[j];
            // La transaccion guarda la fecha como DD-MM-AAAA (formato del
            // enunciado); la Tabla Hash de reservas la maneja como
            // AAAA-MM-DD (asi se compara contra "hoy" para VALIDA/VENCIDA).
            std::string fechaReserva = t.fechaPago;
            if (fechaReserva.size() == 10 && fechaReserva[2] == '-' && fechaReserva[5] == '-') {
                fechaReserva = fechaReserva.substr(6, 4) + "-" + fechaReserva.substr(3, 2) + "-" + fechaReserva.substr(0, 2);
            }
            if (t.estado == "CANCELADA") {
                // La cancelacion siempre va en un bloque posterior al pago,
                // asi que la reserva ya se inserto: se retira de la tabla.
                tabla.eliminar(t.codigoReserva);
                continue;
            }
            Reserva r(t.codigoReserva, t.codigoFuncion, t.idCliente, t.fila, t.columna, fechaReserva);
            tabla.insertar(r);
        }
    }
}

ArregloDinamico<Transaccion> Blockchain::historialDeCliente(const std::string& idCliente) const {
    ArregloDinamico<Transaccion> resultado;
    for (NodoBloque* n = cabeza; n != nullptr; n = n->siguiente) {
        for (int i = 0; i < n->dato.data.getTamanio(); i++) {
            if (n->dato.data[i].idCliente == idCliente) {
                resultado.agregarFinal(n->dato.data[i]);
            }
        }
    }
    return resultado;
}

Blockchain::Comprobante Blockchain::buscarTransaccion(const std::string& codigoTransaccion, const std::string& estado) const {
    Comprobante resultado;

    for (NodoBloque* n = cabeza; n != nullptr; n = n->siguiente) {
        // De atras hacia adelante: si el mismo codigo aparece pagado y
        // cancelado en un mismo bloque, gana el registro mas reciente.
        for (int i = n->dato.data.getTamanio() - 1; i >= 0; i--) {
            if (n->dato.data[i].codigoTransaccion == codigoTransaccion &&
                (estado.empty() || n->dato.data[i].estado == estado)) {
                resultado.encontrado = true;
                resultado.transaccion = n->dato.data[i];
                resultado.indiceBloque = n->dato.index;
                resultado.hashBloque = n->dato.hash;
                return resultado;
            }
        }
    }

    return resultado;
}

bool Blockchain::buscarPagoDeReserva(const std::string& codigoReserva, Transaccion& salida) const {
    for (int i = pendientes.getTamanio() - 1; i >= 0; i--) {
        if (pendientes[i].codigoReserva == codigoReserva && pendientes[i].estado != "CANCELADA") {
            salida = pendientes[i];
            return true;
        }
    }
    for (NodoBloque* n = cabeza; n != nullptr; n = n->siguiente) {
        for (int i = n->dato.data.getTamanio() - 1; i >= 0; i--) {
            const Transaccion& t = n->dato.data[i];
            if (t.codigoReserva == codigoReserva && t.estado != "CANCELADA") {
                salida = t;
                return true;
            }
        }
    }
    return false;
}

Blockchain::ResumenVentas Blockchain::calcularVentasTotales() const {
    ResumenVentas resumen;

    for (NodoBloque* n = cabeza; n != nullptr; n = n->siguiente) {
        for (int i = 0; i < n->dato.data.getTamanio(); i++) {
            // Una transaccion CANCELADA anula la compra original (mismo
            // monto), por eso resta en vez de sumar.
            int signo = (n->dato.data[i].estado == "CANCELADA") ? -1 : 1;
            resumen.totalBoletos += signo;
            resumen.montoTotal += signo * n->dato.data[i].monto;
        }
    }

    return resumen;
}

// ---------------------------------------------------------------------
// Integridad, codigos y desglose de ventas
// ---------------------------------------------------------------------

bool Blockchain::bloqueEsIntegro(const Bloque& b) {
    return b.esValido() && ArbolMerkle::calcularRaiz(b.data) == b.rootMerkle;
}

void Blockchain::registrarEnCadenaDisco(const Bloque& b, bool integro) {
    // El bloque con ese INDEX reemplaza al que hubiera en disco con el
    // mismo indice (caso: se genero un bloque nuevo tras descartar uno
    // manipulado, que sobreescribe su archivo); si no existia, se agrega.
    if (b.index >= 0 && b.index < cadenaEnDisco.getTamanio()) {
        cadenaEnDisco[b.index] = b;
        cadenaEnDiscoIntegra[b.index] = integro;
    } else {
        cadenaEnDisco.agregarFinal(b);
        cadenaEnDiscoIntegra.agregarFinal(integro);
    }
}

std::string Blockchain::generarCodigoTransaccion() const {
    int n = 1;
    while (true) {
        char buf[16];
        std::snprintf(buf, sizeof(buf), "T%03d", n);
        std::string candidato(buf);

        bool usado = buscarTransaccion(candidato).encontrado;
        for (int i = 0; i < pendientes.getTamanio() && !usado; i++) {
            if (pendientes[i].codigoTransaccion == candidato) usado = true;
        }
        if (!usado) return candidato;
        n++;
    }
}

ArregloDinamico<Transaccion> Blockchain::getPendientes() const {
    return pendientes;
}

const ArregloDinamico<Bloque>& Blockchain::getCadenaEnDisco() const {
    return cadenaEnDisco;
}

const ArregloDinamico<bool>& Blockchain::getCadenaEnDiscoIntegra() const {
    return cadenaEnDiscoIntegra;
}

ArregloDinamico<Blockchain::LineaDesglose> Blockchain::desglosar(bool porFuncion) const {
    ArregloDinamico<LineaDesglose> lineas;

    for (NodoBloque* n = cabeza; n != nullptr; n = n->siguiente) {
        for (int i = 0; i < n->dato.data.getTamanio(); i++) {
            const Transaccion& t = n->dato.data[i];
            const std::string& clave = porFuncion ? t.codigoFuncion : t.fechaPago;

            int pos = -1;
            for (int j = 0; j < lineas.getTamanio(); j++) {
                if (lineas[j].clave == clave) { pos = j; break; }
            }
            if (pos == -1) {
                LineaDesglose nueva;
                nueva.clave = clave;
                lineas.agregarFinal(nueva);
                pos = lineas.getTamanio() - 1;
            }
            int signo = (t.estado == "CANCELADA") ? -1 : 1;
            lineas[pos].boletos += signo;
            lineas[pos].monto += signo * t.monto;
        }
    }

    // Orden alfabetico por clave (insercion), para una salida estable.
    for (int i = 1; i < lineas.getTamanio(); i++) {
        LineaDesglose actual = lineas[i];
        int j = i - 1;
        while (j >= 0 && lineas[j].clave > actual.clave) {
            lineas[j + 1] = lineas[j];
            j--;
        }
        lineas[j + 1] = actual;
    }
    return lineas;
}

ArregloDinamico<Blockchain::LineaDesglose> Blockchain::desglosePorFuncion() const {
    return desglosar(true);
}

ArregloDinamico<Blockchain::LineaDesglose> Blockchain::desglosePorFecha() const {
    return desglosar(false);
}
