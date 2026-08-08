#ifndef LISTA_DOBLE_BENEFICIOS_H
#define LISTA_DOBLE_BENEFICIOS_H

#include "Beneficio.h"

// Nodo de la lista de beneficios. A diferencia de NodoPelicula,
// aquí no necesitamos orden ni búsqueda por código: los beneficios
// simplemente se agregan en el orden en que el administrador los crea.
struct NodoBeneficio {
    Beneficio dato;
    NodoBeneficio* anterior;
    NodoBeneficio* siguiente;

    NodoBeneficio(const Beneficio& b)
        : dato(b), anterior(nullptr), siguiente(nullptr) {}
};

// Lista doblemente enlazada de beneficios. Cada Promocion tiene
// una instancia propia de esta clase.
class ListaDobleBeneficios {
private:
    NodoBeneficio* cabeza;
    NodoBeneficio* cola;
    int tamanio;

public:
    ListaDobleBeneficios();
    ~ListaDobleBeneficios();

    // Constructor de copia y operador de asignación: son necesarios
    // porque Promocion va a contener un ListaDobleBeneficios como
    // miembro, y si Promocion se copia (ej: al insertarla en la lista
    // circular), también hay que copiar profundamente sus beneficios
    // para no compartir memoria entre dos promociones distintas.
    ListaDobleBeneficios(const ListaDobleBeneficios& otra);
    ListaDobleBeneficios& operator=(const ListaDobleBeneficios& otra);

    void agregar(const Beneficio& b);
    bool eliminar(int indice); // elimina por posición (0-indexado)
    void imprimir() const;

    int getTamanio() const;
    bool estaVacia() const;
    NodoBeneficio* getCabeza() const;

private:
    void copiarDesde(const ListaDobleBeneficios& otra);
    void liberar();
};

#endif // LISTA_DOBLE_BENEFICIOS_H
