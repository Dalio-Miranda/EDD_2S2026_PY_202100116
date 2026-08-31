#include "TablaHashReservas.h"

TablaHashReservas::TablaHashReservas(int tamanioTabla)
    : buckets(tamanioTabla, nullptr), tamanioTabla(tamanioTabla), cantidadReservas(0) {}

TablaHashReservas::~TablaHashReservas() {
    for (NodoHash* cabeza : buckets) {
        NodoHash* actual = cabeza;
        while (actual != nullptr) {
            NodoHash* siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        }
    }
}

// Hash tipo djb2: h = h*33 + c, acumulado sobre cada caracter. Simple,
// rapido y con buena distribucion para claves cortas tipo "R001".
int TablaHashReservas::funcionHash(const std::string& clave) const {
    unsigned long h = 5381;
    for (unsigned char c : clave) {
        h = ((h << 5) + h) + c; // h*33 + c
    }
    return static_cast<int>(h % static_cast<unsigned long>(tamanioTabla));
}

int TablaHashReservas::calcularIndice(const std::string& codigoReserva) const {
    return funcionHash(codigoReserva);
}

bool TablaHashReservas::insertar(const Reserva& r) {
    int indice = funcionHash(r.codigoReserva);

    // Verificar que no exista ya (recorrer la lista del bucket).
    for (NodoHash* actual = buckets[indice]; actual != nullptr; actual = actual->siguiente) {
        if (actual->dato.codigoReserva == r.codigoReserva) {
            return false; // codigo_reserva duplicado
        }
    }

    // Insertar al inicio de la lista del bucket (O(1)).
    NodoHash* nuevo = new NodoHash(r);
    nuevo->siguiente = buckets[indice];
    buckets[indice] = nuevo;

    cantidadReservas++;
    return true;
}

bool TablaHashReservas::eliminar(const std::string& codigoReserva) {
    int indice = funcionHash(codigoReserva);

    NodoHash* actual = buckets[indice];
    NodoHash* anterior = nullptr;

    while (actual != nullptr) {
        if (actual->dato.codigoReserva == codigoReserva) {
            if (anterior == nullptr) {
                buckets[indice] = actual->siguiente;
            } else {
                anterior->siguiente = actual->siguiente;
            }
            delete actual;
            cantidadReservas--;
            return true;
        }
        anterior = actual;
        actual = actual->siguiente;
    }

    return false; // no existia
}

Reserva* TablaHashReservas::buscar(const std::string& codigoReserva) {
    int indice = funcionHash(codigoReserva);
    for (NodoHash* actual = buckets[indice]; actual != nullptr; actual = actual->siguiente) {
        if (actual->dato.codigoReserva == codigoReserva) {
            return &actual->dato;
        }
    }
    return nullptr;
}

bool TablaHashReservas::existe(const std::string& codigoReserva) const {
    int indice = funcionHash(codigoReserva);
    for (NodoHash* actual = buckets[indice]; actual != nullptr; actual = actual->siguiente) {
        if (actual->dato.codigoReserva == codigoReserva) {
            return true;
        }
    }
    return false;
}

int TablaHashReservas::getCantidadReservas() const { return cantidadReservas; }
int TablaHashReservas::getTamanioTabla() const { return tamanioTabla; }
bool TablaHashReservas::estaVacio() const { return cantidadReservas == 0; }

std::vector<Reserva> TablaHashReservas::listarTodas() const {
    std::vector<Reserva> salida;
    salida.reserve(cantidadReservas);
    for (NodoHash* cabeza : buckets) {
        for (NodoHash* actual = cabeza; actual != nullptr; actual = actual->siguiente) {
            salida.push_back(actual->dato);
        }
    }
    return salida;
}

int TablaHashReservas::contarBucketsOcupados() const {
    int contador = 0;
    for (NodoHash* cabeza : buckets) {
        if (cabeza != nullptr) contador++;
    }
    return contador;
}

int TablaHashReservas::contarColisiones() const {
    // Cada reserva mas alla de la primera en un bucket ocupado es una
    // colision (definicion estandar para tablas con encadenamiento).
    return cantidadReservas - contarBucketsOcupados();
}

NodoHash* TablaHashReservas::getBucket(int indice) const {
    if (indice < 0 || indice >= tamanioTabla) return nullptr;
    return buckets[indice];
}
