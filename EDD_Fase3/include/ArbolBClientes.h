#ifndef ARBOL_B_CLIENTES_H
#define ARBOL_B_CLIENTES_H

#include <string>
#include "ArregloDinamico.h"
#include "Cliente.h"

// Arbol B de orden 4: grado minimo t = 2.
//   - Maximo de claves por nodo:  2t - 1 = 3
//   - Minimo de claves por nodo:  t - 1  = 1   (la raiz puede tener menos)
//   - Maximo de hijos por nodo:   2t     = 4
//   - Minimo de hijos por nodo:   t      = 2   (salvo la raiz)
constexpr int GRADO_MINIMO_B = 2;
constexpr int MAX_CLAVES_B = 2 * GRADO_MINIMO_B - 1; // 3
constexpr int MIN_CLAVES_B = GRADO_MINIMO_B - 1;     // 1

struct NodoB {
    ArregloDinamico<Cliente> claves;   // hasta MAX_CLAVES_B, ordenadas por Cliente::id
    ArregloDinamico<NodoB*> hijos;     // hasta MAX_CLAVES_B + 1, vacio si es hoja
    bool esHoja;

    explicit NodoB(bool hoja) : esHoja(hoja) {}
};

// Arbol B de orden 4 para administrar clientes, indexado por "id".
// Implementacion 100% manual: ni las claves/hijos de cada nodo, ni
// ninguna otra parte de la estructura, usan std::vector ni ningun
// otro contenedor de la STL -- se usa ArregloDinamico (arreglo
// dinamico propio, ver ArregloDinamico.h) en su lugar.
class ArbolBClientes {
private:
    NodoB* raiz;
    int tamanio;

    NodoB* buscarRec(NodoB* nodo, const std::string& id, int& indiceOut) const;

    void dividirHijo(NodoB* padre, int indiceHijo);
    void insertarNoLleno(NodoB* nodo, const Cliente& c);

    void eliminarRec(NodoB* nodo, const std::string& id);
    int encontrarIndiceClave(NodoB* nodo, const std::string& id) const;
    Cliente obtenerPredecesor(NodoB* nodo, int indice) const;
    Cliente obtenerSucesor(NodoB* nodo, int indice) const;
    void rellenarHijo(NodoB* nodo, int indiceHijo);
    void prestarDeAnterior(NodoB* nodo, int indiceHijo);
    void prestarDeSiguiente(NodoB* nodo, int indiceHijo);
    void fusionarConSiguiente(NodoB* nodo, int indiceHijo);

    void liberar(NodoB* nodo);
    void inordenRec(NodoB* nodo, ArregloDinamico<Cliente>& salida) const;

public:
    ArbolBClientes();
    ~ArbolBClientes();

    ArbolBClientes(const ArbolBClientes&) = delete;
    ArbolBClientes& operator=(const ArbolBClientes&) = delete;

    bool insertar(const Cliente& c);
    bool eliminar(const std::string& id);
    bool editar(const std::string& id, const Cliente& datosNuevos);

    Cliente* buscar(const std::string& id);
    bool existe(const std::string& id) const;

    int getTamanio() const;
    bool estaVacio() const;

    ArregloDinamico<Cliente> listarClientes() const;

    NodoB* getRaiz() const;
};

#endif // ARBOL_B_CLIENTES_H
