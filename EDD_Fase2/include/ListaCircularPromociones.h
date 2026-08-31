#ifndef LISTA_CIRCULAR_PROMOCIONES_H
#define LISTA_CIRCULAR_PROMOCIONES_H

#include <string>
#include "Promocion.h"

// Nodo de la lista circular simple. Solo tiene puntero "siguiente"
// (es SIMPLE, no doble), y el último nodo apunta de vuelta al
// primero en vez de a nullptr (por eso es CIRCULAR).
struct NodoPromocion {
    Promocion dato;
    NodoPromocion* siguiente;

    NodoPromocion(const Promocion& p)
        : dato(p), siguiente(nullptr) {}
};

// Lista circular simple que agrupa todas las promociones del sistema.
// Permite recorrerlas cíclicamente (útil, por ejemplo, para mostrarlas
// en un carrusel de promociones en la interfaz de cliente).
class ListaCircularPromociones {
private:
    NodoPromocion* ultimo; // guardamos el ÚLTIMO nodo, no la cabeza;
                           // así "ultimo->siguiente" siempre es el
                           // primer nodo, e insertar al final es O(1)
    int tamanio;

public:
    ListaCircularPromociones();
    ~ListaCircularPromociones();

    void agregar(const Promocion& p);
    bool eliminar(const std::string& codigo);
    Promocion* buscar(const std::string& codigo);

    // Recorre "n" nodos a partir del primero, dando vueltas si
    // hace falta. Sirve para demostrar/probar que la lista es
    // efectivamente circular.
    void recorrerCiclo(int n) const;

    void imprimir() const;

    int getTamanio() const;
    bool estaVacia() const;

    // Puntero al primer nodo (ultimo->siguiente), útil para el
    // reporte de Graphviz más adelante.
    NodoPromocion* getPrimero() const;
};

#endif // LISTA_CIRCULAR_PROMOCIONES_H
