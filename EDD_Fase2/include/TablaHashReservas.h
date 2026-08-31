#ifndef TABLA_HASH_RESERVAS_H
#define TABLA_HASH_RESERVAS_H

#include <string>
#include <vector>
#include "Reserva.h"

// Nodo de la lista enlazada de cada bucket (manejo de colisiones por
// encadenamiento separado).
struct NodoHash {
    Reserva dato;
    NodoHash* siguiente;

    explicit NodoHash(const Reserva& r) : dato(r), siguiente(nullptr) {}
};

// Tabla Hash general de reservas, indexada por codigo_reserva.
// Implementacion 100% manual (sin std::unordered_map ni ninguna tabla
// hash de la STL), con manejo de colisiones por encadenamiento
// separado -- esto es justamente lo que pide poder visualizar el
// Reporte 4 (buckets + listas enlazadas de colisiones).
class TablaHashReservas {
private:
    static constexpr int TAMANIO_DEFECTO = 17; // primo, reduce colisiones

    std::vector<NodoHash*> buckets;
    int tamanioTabla;
    int cantidadReservas;

    // Funcion hash tipo djb2 sobre el codigo_reserva (string),
    // acotada al tamanio de la tabla.
    int funcionHash(const std::string& clave) const;

public:
    explicit TablaHashReservas(int tamanioTabla = TAMANIO_DEFECTO);
    ~TablaHashReservas();

    TablaHashReservas(const TablaHashReservas&) = delete;
    TablaHashReservas& operator=(const TablaHashReservas&) = delete;

    // Inserta una reserva nueva. Retorna false si el codigo_reserva
    // ya existe.
    bool insertar(const Reserva& r);

    // Elimina la reserva con ese codigo. Usado tanto por "cancelar
    // reserva" del cliente como por la eliminacion en cascada al
    // borrar un cliente del Arbol B.
    bool eliminar(const std::string& codigoReserva);

    Reserva* buscar(const std::string& codigoReserva);
    bool existe(const std::string& codigoReserva) const;

    int getCantidadReservas() const;
    int getTamanioTabla() const;
    bool estaVacio() const;

    // Todas las reservas, recorriendo bucket por bucket (para "ver mi
    // historial de reservas" filtrando despues por id_cliente, o para
    // listados administrativos).
    std::vector<Reserva> listarTodas() const;

    // ---- estadisticas para el Reporte 4 (Tabla Hash de Reservas) ----
    int contarBucketsOcupados() const;
    int contarColisiones() const; // cantidadReservas - bucketsOcupados

    // ---- acceso a la estructura interna, para ReportesGraphviz ----
    int calcularIndice(const std::string& codigoReserva) const;
    NodoHash* getBucket(int indice) const;
};

#endif // TABLA_HASH_RESERVAS_H
