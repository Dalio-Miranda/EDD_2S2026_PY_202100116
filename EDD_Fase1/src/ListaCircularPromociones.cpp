#include "ListaCircularPromociones.h"
#include <iostream>

ListaCircularPromociones::ListaCircularPromociones()
    : ultimo(nullptr), tamanio(0) {}

ListaCircularPromociones::~ListaCircularPromociones() {
    if (ultimo == nullptr) return;

    // Rompemos el ciclo primero para poder recorrer y liberar
    // linealmente sin quedar en un bucle infinito.
    NodoPromocion* primero = ultimo->siguiente;
    NodoPromocion* actual = primero;

    for (int i = 0; i < tamanio; i++) {
        NodoPromocion* siguiente = actual->siguiente;
        delete actual;
        actual = siguiente;
    }
    ultimo = nullptr;
}

void ListaCircularPromociones::agregar(const Promocion& p) {
    NodoPromocion* nuevo = new NodoPromocion(p);

    if (ultimo == nullptr) {
        // Único nodo: se apunta a sí mismo, cerrando el ciclo.
        nuevo->siguiente = nuevo;
        ultimo = nuevo;
    } else {
        nuevo->siguiente = ultimo->siguiente; // apunta al primero actual
        ultimo->siguiente = nuevo;            // el viejo último ahora apunta al nuevo
        ultimo = nuevo;                       // el nuevo pasa a ser el último
    }
    tamanio++;
}

bool ListaCircularPromociones::eliminar(const std::string& codigo) {
    if (ultimo == nullptr) return false;

    // Caso especial: un solo nodo en la lista
    if (tamanio == 1) {
        if (ultimo->dato.codigo == codigo) {
            delete ultimo;
            ultimo = nullptr;
            tamanio = 0;
            return true;
        }
        return false;
    }

    NodoPromocion* anterior = ultimo;
    NodoPromocion* actual = ultimo->siguiente; // el primero

    for (int i = 0; i < tamanio; i++) {
        if (actual->dato.codigo == codigo) {
            anterior->siguiente = actual->siguiente;
            if (actual == ultimo) {
                ultimo = anterior; // eliminamos justo el último
            }
            delete actual;
            tamanio--;
            return true;
        }
        anterior = actual;
        actual = actual->siguiente;
    }

    return false; // no se encontró tras dar la vuelta completa
}

Promocion* ListaCircularPromociones::buscar(const std::string& codigo) {
    if (ultimo == nullptr) return nullptr;

    NodoPromocion* actual = ultimo->siguiente; // el primero
    for (int i = 0; i < tamanio; i++) {
        if (actual->dato.codigo == codigo) {
            return &(actual->dato);
        }
        actual = actual->siguiente;
    }
    return nullptr;
}

void ListaCircularPromociones::recorrerCiclo(int n) const {
    if (ultimo == nullptr) {
        std::cout << "No hay promociones registradas." << std::endl;
        return;
    }

    NodoPromocion* actual = ultimo->siguiente; // el primero
    for (int i = 0; i < n; i++) {
        std::cout << "  [" << i << "] " << actual->dato.codigo
                   << " - " << actual->dato.nombre << std::endl;
        actual = actual->siguiente; // si se pasa del final, vuelve al primero solo
    }
}

void ListaCircularPromociones::imprimir() const {
    if (ultimo == nullptr) {
        std::cout << "No hay promociones registradas." << std::endl;
        return;
    }

    std::cout << "===== Promociones (" << tamanio << ") =====" << std::endl;
    NodoPromocion* actual = ultimo->siguiente; // el primero
    for (int i = 0; i < tamanio; i++) {
        const Promocion& p = actual->dato;
        std::cout << p.codigo << " | " << p.nombre
                   << " | Vigencia: " << p.fechaInicio << " a " << p.fechaFin
                   << " | Dias: ";
        for (size_t j = 0; j < p.diasAplicables.size(); j++) {
            std::cout << p.diasAplicables[j];
            if (j + 1 < p.diasAplicables.size()) std::cout << ", ";
        }
        std::cout << std::endl;
        std::cout << "  Beneficios:" << std::endl;
        p.beneficios.imprimir();

        actual = actual->siguiente;
    }
}

int ListaCircularPromociones::getTamanio() const {
    return tamanio;
}

bool ListaCircularPromociones::estaVacia() const {
    return tamanio == 0;
}

NodoPromocion* ListaCircularPromociones::getPrimero() const {
    if (ultimo == nullptr) return nullptr;
    return ultimo->siguiente;
}
