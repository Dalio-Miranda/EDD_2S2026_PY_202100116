#include "TablaHashReservas.h"

TablaHashReservas::TablaHashReservas(int tamanioTabla)
    : tamanioTabla(tamanioTabla), cantidadReservas(0) {
    buckets = new NodoHash*[tamanioTabla];
    for (int i = 0; i < tamanioTabla; i++) {
        buckets[i] = nullptr;
    }
}

TablaHashReservas::~TablaHashReservas() {
    for (int i = 0; i < tamanioTabla; i++) {
        NodoHash* actual = buckets[i];
        while (actual != nullptr) {
            NodoHash* siguiente = actual->siguiente;
            delete actual;
            actual = siguiente;
        }
    }
    delete[] buckets;
}

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

    for (NodoHash* actual = buckets[indice]; actual != nullptr; actual = actual->siguiente) {
        if (actual->dato.codigoReserva == r.codigoReserva) {
            return false;
        }
    }

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

    return false;
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

ArregloDinamico<Reserva> TablaHashReservas::listarTodas() const {
    ArregloDinamico<Reserva> salida;
    for (int i = 0; i < tamanioTabla; i++) {
        for (NodoHash* actual = buckets[i]; actual != nullptr; actual = actual->siguiente) {
            salida.agregarFinal(actual->dato);
        }
    }
    return salida;
}

int TablaHashReservas::contarBucketsOcupados() const {
    int contador = 0;
    for (int i = 0; i < tamanioTabla; i++) {
        if (buckets[i] != nullptr) contador++;
    }
    return contador;
}

int TablaHashReservas::contarColisiones() const {
    return cantidadReservas - contarBucketsOcupados();
}

NodoHash* TablaHashReservas::getBucket(int indice) const {
    if (indice < 0 || indice >= tamanioTabla) return nullptr;
    return buckets[indice];
}
