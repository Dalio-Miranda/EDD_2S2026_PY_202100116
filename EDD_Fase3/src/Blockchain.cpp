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
    int validos = 0;
    for (int i = 0; i < bloquesEnDisco.getTamanio(); i++) {
        const Bloque& actual = bloquesEnDisco[i];

        bool prevOk = (i == 0)
            ? (actual.previousHash == "0000")
            : (actual.previousHash == bloquesEnDisco[i - 1].hash);

        bool hashOk = actual.esValido();

        std::string raizRecalculada = ArbolMerkle::calcularRaiz(actual.data);
        bool merkleOk = (raizRecalculada == actual.rootMerkle);

        if (!prevOk || !hashOk || !merkleOk) {
            resultado.huboManipulacion = true;
            resultado.indiceDelPrimerManipulado = actual.index;
            break;
        }
        validos++;
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
            Reserva r(t.codigoReserva, t.codigoFuncion, t.idCliente, t.fila, t.columna, t.fechaPago);
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

Blockchain::Comprobante Blockchain::buscarTransaccion(const std::string& codigoTransaccion) const {
    Comprobante resultado;

    for (NodoBloque* n = cabeza; n != nullptr; n = n->siguiente) {
        for (int i = 0; i < n->dato.data.getTamanio(); i++) {
            if (n->dato.data[i].codigoTransaccion == codigoTransaccion) {
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

Blockchain::ResumenVentas Blockchain::calcularVentasTotales() const {
    ResumenVentas resumen;

    for (NodoBloque* n = cabeza; n != nullptr; n = n->siguiente) {
        for (int i = 0; i < n->dato.data.getTamanio(); i++) {
            resumen.totalBoletos++;
            resumen.montoTotal += n->dato.data[i].monto;
        }
    }

    return resumen;
}
