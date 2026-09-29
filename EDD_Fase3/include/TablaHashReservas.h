#ifndef TABLA_HASH_RESERVAS_H
#define TABLA_HASH_RESERVAS_H

#include <string>
#include "ArregloDinamico.h"
#include "Reserva.h"

struct NodoHash {
    Reserva dato;
    NodoHash* siguiente;

    explicit NodoHash(const Reserva& r) : dato(r), siguiente(nullptr) {}
};

// Tabla Hash general de reservas, indexada por codigo_reserva.
// Implementacion 100% manual: el arreglo de buckets es un arreglo
// dinamico CRUDO (new[]/delete[]), no un std::vector -- su tamanio es
// fijo desde la construccion, asi que un arreglo simple basta y no
// hace falta la logica de redimensionamiento de ArregloDinamico.
// El manejo de colisiones es por encadenamiento separado (lista
// enlazada manual por bucket).
class TablaHashReservas {
private:
    static constexpr int TAMANIO_DEFECTO = 17; // primo, reduce colisiones

    NodoHash** buckets; // arreglo crudo de punteros, tamanio = tamanioTabla
    int tamanioTabla;
    int cantidadReservas;

    int funcionHash(const std::string& clave) const;

public:
    explicit TablaHashReservas(int tamanioTabla = TAMANIO_DEFECTO);
    ~TablaHashReservas();

    TablaHashReservas(const TablaHashReservas&) = delete;
    TablaHashReservas& operator=(const TablaHashReservas&) = delete;

    bool insertar(const Reserva& r);
    bool eliminar(const std::string& codigoReserva);

    Reserva* buscar(const std::string& codigoReserva);
    bool existe(const std::string& codigoReserva) const;

    int getCantidadReservas() const;
    int getTamanioTabla() const;
    bool estaVacio() const;

    ArregloDinamico<Reserva> listarTodas() const;

    int contarBucketsOcupados() const;
    int contarColisiones() const;

    int calcularIndice(const std::string& codigoReserva) const;
    NodoHash* getBucket(int indice) const;
};

#endif // TABLA_HASH_RESERVAS_H
