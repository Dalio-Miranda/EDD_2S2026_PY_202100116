#include "ListaDobleBeneficios.h"
#include <iostream>
 
ListaDobleBeneficios::ListaDobleBeneficios()
    : cabeza(nullptr), cola(nullptr), tamanio(0) {}
 
ListaDobleBeneficios::~ListaDobleBeneficios() {
    liberar();
}
 
ListaDobleBeneficios::ListaDobleBeneficios(const ListaDobleBeneficios& otra)
    : cabeza(nullptr), cola(nullptr), tamanio(0) {
    copiarDesde(otra);
}
 
ListaDobleBeneficios& ListaDobleBeneficios::operator=(const ListaDobleBeneficios& otra) {
    if (this != &otra) {
        liberar();
        copiarDesde(otra);
    }
    return *this;
}
 
void ListaDobleBeneficios::liberar() {
    NodoBeneficio* actual = cabeza;
    while (actual != nullptr) {
        NodoBeneficio* siguiente = actual->siguiente;
        delete actual;
        actual = siguiente;
    }
    cabeza = nullptr;
    cola = nullptr;
    tamanio = 0;
}
 
void ListaDobleBeneficios::copiarDesde(const ListaDobleBeneficios& otra) {
    for (NodoBeneficio* n = otra.cabeza; n != nullptr; n = n->siguiente) {
        agregar(n->dato);
    }
}
 
void ListaDobleBeneficios::agregar(const Beneficio& b) {
    NodoBeneficio* nuevo = new NodoBeneficio(b);
 
    if (cabeza == nullptr) {
        cabeza = nuevo;
        cola = nuevo;
    } else {
        nuevo->anterior = cola;
        cola->siguiente = nuevo;
        cola = nuevo;
    }
    tamanio++;
}
 
bool ListaDobleBeneficios::eliminar(int indice) {
    if (indice < 0 || indice >= tamanio) return false;
 
    NodoBeneficio* actual = cabeza;
    for (int i = 0; i < indice; i++) {
        actual = actual->siguiente;
    }
 
    if (actual->anterior != nullptr) {
        actual->anterior->siguiente = actual->siguiente;
    } else {
        cabeza = actual->siguiente;
    }
 
    if (actual->siguiente != nullptr) {
        actual->siguiente->anterior = actual->anterior;
    } else {
        cola = actual->anterior;
    }
 
    delete actual;
    tamanio--;
    return true;
}
 
void ListaDobleBeneficios::imprimir() const {
    if (cabeza == nullptr) {
        std::cout << "    (sin beneficios)" << std::endl;
        return;
    }
    for (NodoBeneficio* actual = cabeza; actual != nullptr; actual = actual->siguiente) {
        std::cout << "    - " << actual->dato.tipo
                   << " | " << actual->dato.descripcion
                   << " | valor: " << actual->dato.valor
                   << std::endl;
    }
}
 
int ListaDobleBeneficios::getTamanio() const {
    return tamanio;
}
 
bool ListaDobleBeneficios::estaVacia() const {
    return tamanio == 0;
}
 
NodoBeneficio* ListaDobleBeneficios::getCabeza() const {
    return cabeza;
}