#include "ListaCircularSolicitudes.h"
#include <iostream>
#include <ctime>

ListaCircularSolicitudes::ListaCircularSolicitudes()
    : ultimo(nullptr), tamanio(0), siguienteNumero(1) {}

ListaCircularSolicitudes::~ListaCircularSolicitudes() {
    if (ultimo == nullptr) return;

    NodoSolicitud* primero = ultimo->siguiente;
    NodoSolicitud* actual = primero;

    for (int i = 0; i < tamanio; i++) {
        NodoSolicitud* siguiente = actual->siguiente;
        delete actual;
        actual = siguiente;
    }
    ultimo = nullptr;
}

std::string ListaCircularSolicitudes::obtenerFechaActual() {
    std::time_t t = std::time(nullptr);
    std::tm* tmPtr = std::localtime(&t);

    char buffer[11]; // "AAAA-MM-DD\0"
    std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d",
                  tmPtr->tm_year + 1900, tmPtr->tm_mon + 1, tmPtr->tm_mday);
    return std::string(buffer);
}

int ListaCircularSolicitudes::registrarSolicitud(const std::string& nombreCliente,
                                                  const std::string& telefono,
                                                  const std::string& tipoSolicitud,
                                                  const std::string& descripcion) {
    Solicitud s;
    s.numero = siguienteNumero++;
    s.nombreCliente = nombreCliente;
    s.telefono = telefono;
    s.tipoSolicitud = tipoSolicitud;
    s.descripcion = descripcion;
    s.fechaSolicitud = obtenerFechaActual();
    s.estado = "Pendiente";

    NodoSolicitud* nuevo = new NodoSolicitud(s);

    if (ultimo == nullptr) {
        nuevo->siguiente = nuevo;
        nuevo->anterior = nuevo;
        ultimo = nuevo;
    } else {
        NodoSolicitud* primero = ultimo->siguiente;
        nuevo->siguiente = primero;
        nuevo->anterior = ultimo;
        ultimo->siguiente = nuevo;
        primero->anterior = nuevo;
        ultimo = nuevo;
    }
    tamanio++;

    return s.numero;
}

bool ListaCircularSolicitudes::cambiarEstado(int numero, const std::string& nuevoEstado) {
    Solicitud* s = buscarPorNumero(numero);
    if (s == nullptr) return false;
    s->estado = nuevoEstado;
    return true;
}

bool ListaCircularSolicitudes::eliminar(int numero) {
    if (ultimo == nullptr) return false;

    if (tamanio == 1) {
        if (ultimo->dato.numero == numero) {
            delete ultimo;
            ultimo = nullptr;
            tamanio = 0;
            return true;
        }
        return false;
    }

    NodoSolicitud* actual = ultimo->siguiente; // el primero

    for (int i = 0; i < tamanio; i++) {
        if (actual->dato.numero == numero) {
            actual->anterior->siguiente = actual->siguiente;
            actual->siguiente->anterior = actual->anterior;

            if (actual == ultimo) {
                ultimo = actual->anterior;
            }

            delete actual;
            tamanio--;
            return true;
        }
        actual = actual->siguiente;
    }

    return false;
}

Solicitud* ListaCircularSolicitudes::buscarPorNumero(int numero) {
    if (ultimo == nullptr) return nullptr;

    NodoSolicitud* actual = ultimo->siguiente;
    for (int i = 0; i < tamanio; i++) {
        if (actual->dato.numero == numero) {
            return &(actual->dato);
        }
        actual = actual->siguiente;
    }
    return nullptr;
}

std::vector<Solicitud> ListaCircularSolicitudes::buscarPorTelefono(const std::string& telefono) {
    std::vector<Solicitud> resultado;
    if (ultimo == nullptr) return resultado;

    NodoSolicitud* actual = ultimo->siguiente;
    for (int i = 0; i < tamanio; i++) {
        if (actual->dato.telefono == telefono) {
            resultado.push_back(actual->dato);
        }
        actual = actual->siguiente;
    }
    return resultado;
}

int ListaCircularSolicitudes::contarPendientes() const {
    if (ultimo == nullptr) return 0;

    int contador = 0;
    NodoSolicitud* actual = ultimo->siguiente;
    for (int i = 0; i < tamanio; i++) {
        if (actual->dato.estado == "Pendiente") {
            contador++;
        }
        actual = actual->siguiente;
    }
    return contador;
}

void ListaCircularSolicitudes::imprimirPendientes() const {
    if (ultimo == nullptr) {
        std::cout << "No hay solicitudes registradas." << std::endl;
        return;
    }

    std::cout << "===== Solicitudes pendientes =====" << std::endl;
    int encontradas = 0;
    NodoSolicitud* actual = ultimo->siguiente;
    for (int i = 0; i < tamanio; i++) {
        if (actual->dato.estado == "Pendiente") {
            const Solicitud& s = actual->dato;
            std::cout << "#" << s.numero << " | " << s.nombreCliente
                       << " | Tel: " << s.telefono
                       << " | Tipo: " << s.tipoSolicitud
                       << " | " << s.descripcion
                       << " | Fecha: " << s.fechaSolicitud
                       << std::endl;
            encontradas++;
        }
        actual = actual->siguiente;
    }
    if (encontradas == 0) {
        std::cout << "(ninguna pendiente)" << std::endl;
    }
}

void ListaCircularSolicitudes::imprimir() const {
    if (ultimo == nullptr) {
        std::cout << "No hay solicitudes registradas." << std::endl;
        return;
    }

    std::cout << "===== Todas las solicitudes (" << tamanio << ") =====" << std::endl;
    NodoSolicitud* actual = ultimo->siguiente;
    for (int i = 0; i < tamanio; i++) {
        const Solicitud& s = actual->dato;
        std::cout << "#" << s.numero << " | " << s.nombreCliente
                   << " | Tel: " << s.telefono
                   << " | Tipo: " << s.tipoSolicitud
                   << " | " << s.descripcion
                   << " | Fecha: " << s.fechaSolicitud
                   << " | Estado: " << s.estado
                   << std::endl;
        actual = actual->siguiente;
    }
}

int ListaCircularSolicitudes::getTamanio() const {
    return tamanio;
}

bool ListaCircularSolicitudes::estaVacia() const {
    return tamanio == 0;
}

NodoSolicitud* ListaCircularSolicitudes::getPrimero() const {
    if (ultimo == nullptr) return nullptr;
    return ultimo->siguiente;
}