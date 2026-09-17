#ifndef ARREGLO_DINAMICO_H
#define ARREGLO_DINAMICO_H

#include <utility>

// Arreglo dinamico de tamanio variable, implementado manualmente
// (new[]/delete[] y duplicacion de capacidad), para no depender de
// std::vector ni de ningun otro contenedor de la STL. Se usa en todo
// el proyecto en cualquier lugar donde antes hubiera ido un
// std::vector -- tanto dentro de las estructuras (ej. las claves e
// hijos de un nodo del Arbol B, o los buckets de la Tabla Hash) como
// para devolver listas de resultados (recorridos, listados) a la
// interfaz.
template <typename T>
class ArregloDinamico {
private:
    T* datos;
    int cantidad;
    int capacidad;

    void asegurarCapacidad(int minimo) {
        if (capacidad >= minimo) return;
        int nuevaCapacidad = (capacidad == 0) ? 4 : capacidad * 2;
        while (nuevaCapacidad < minimo) nuevaCapacidad *= 2;

        T* nuevoDatos = new T[nuevaCapacidad];
        for (int i = 0; i < cantidad; i++) {
            nuevoDatos[i] = std::move(datos[i]);
        }
        delete[] datos;
        datos = nuevoDatos;
        capacidad = nuevaCapacidad;
    }

public:
    ArregloDinamico() : datos(nullptr), cantidad(0), capacidad(0) {}

    ArregloDinamico(const ArregloDinamico& otro) : datos(nullptr), cantidad(0), capacidad(0) {
        if (otro.capacidad > 0) {
            datos = new T[otro.capacidad];
            capacidad = otro.capacidad;
        }
        for (int i = 0; i < otro.cantidad; i++) {
            datos[i] = otro.datos[i];
        }
        cantidad = otro.cantidad;
    }

    ArregloDinamico& operator=(const ArregloDinamico& otro) {
        if (this == &otro) return *this;
        T* nuevoDatos = (otro.capacidad > 0) ? new T[otro.capacidad] : nullptr;
        for (int i = 0; i < otro.cantidad; i++) {
            nuevoDatos[i] = otro.datos[i];
        }
        delete[] datos;
        datos = nuevoDatos;
        cantidad = otro.cantidad;
        capacidad = otro.capacidad;
        return *this;
    }

    ArregloDinamico(ArregloDinamico&& otro) noexcept
        : datos(otro.datos), cantidad(otro.cantidad), capacidad(otro.capacidad) {
        otro.datos = nullptr;
        otro.cantidad = 0;
        otro.capacidad = 0;
    }

    ArregloDinamico& operator=(ArregloDinamico&& otro) noexcept {
        if (this == &otro) return *this;
        delete[] datos;
        datos = otro.datos;
        cantidad = otro.cantidad;
        capacidad = otro.capacidad;
        otro.datos = nullptr;
        otro.cantidad = 0;
        otro.capacidad = 0;
        return *this;
    }

    ~ArregloDinamico() {
        delete[] datos;
    }

    // Agrega un elemento al final (equivalente a push_back).
    void agregarFinal(const T& valor) {
        asegurarCapacidad(cantidad + 1);
        datos[cantidad] = valor;
        cantidad++;
    }

    // Inserta en una posicion especifica, recorriendo el resto hacia
    // la derecha (equivalente a insert(begin()+indice, valor)).
    void insertarEn(int indice, const T& valor) {
        asegurarCapacidad(cantidad + 1);
        for (int i = cantidad; i > indice; i--) {
            datos[i] = std::move(datos[i - 1]);
        }
        datos[indice] = valor;
        cantidad++;
    }

    // Elimina el elemento en una posicion especifica, recorriendo el
    // resto hacia la izquierda (equivalente a erase(begin()+indice)).
    void eliminarEn(int indice) {
        for (int i = indice; i < cantidad - 1; i++) {
            datos[i] = std::move(datos[i + 1]);
        }
        cantidad--;
    }

    void eliminarUltimo() {
        if (cantidad > 0) cantidad--;
    }

    // Busca y elimina la primera ocurrencia de un valor (requiere que
    // T soporte operator==). Util para quitar un codigo especifico de
    // una lista de referencias (ej. codigosReserva de un Cliente) sin
    // tener que conocer su indice de antemano.
    void eliminarValor(const T& valor) {
        for (int i = 0; i < cantidad; i++) {
            if (datos[i] == valor) {
                eliminarEn(i);
                return;
            }
        }
    }

    void vaciar() {
        cantidad = 0;
    }

    // Fija el tamanio logico a nuevaCantidad, rellenando con T()
    // cuando crece (equivalente a resize()).
    void ajustarTamanio(int nuevaCantidad) {
        asegurarCapacidad(nuevaCantidad);
        for (int i = cantidad; i < nuevaCantidad; i++) {
            datos[i] = T();
        }
        cantidad = nuevaCantidad;
    }

    T& operator[](int indice) { return datos[indice]; }
    const T& operator[](int indice) const { return datos[indice]; }

    T& atras() { return datos[cantidad - 1]; }
    const T& atras() const { return datos[cantidad - 1]; }
    T& frente() { return datos[0]; }
    const T& frente() const { return datos[0]; }

    int getTamanio() const { return cantidad; }
    bool estaVacio() const { return cantidad == 0; }

    // Iteradores basados en puntero crudo: permiten usar range-based
    // for (for (auto& x : arreglo)) igual que con std::vector.
    T* begin() { return datos; }
    T* end() { return datos + cantidad; }
    const T* begin() const { return datos; }
    const T* end() const { return datos + cantidad; }
};

#endif // ARREGLO_DINAMICO_H
