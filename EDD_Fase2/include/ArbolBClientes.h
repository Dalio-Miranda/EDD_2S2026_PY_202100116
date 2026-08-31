#ifndef ARBOL_B_CLIENTES_H
#define ARBOL_B_CLIENTES_H

#include <string>
#include <vector>
#include "Cliente.h"

// Arbol B de orden 4: grado minimo t = 2.
//   - Maximo de claves por nodo:  2t - 1 = 3
//   - Minimo de claves por nodo:  t - 1  = 1   (la raiz puede tener menos)
//   - Maximo de hijos por nodo:   2t     = 4
//   - Minimo de hijos por nodo:   t      = 2   (salvo la raiz)
//
// Esta es la formulacion estandar de grado minimo (CLRS, cap. 18),
// que para t=2 produce exactamente un arbol B de orden 4.
constexpr int GRADO_MINIMO_B = 2;
constexpr int MAX_CLAVES_B = 2 * GRADO_MINIMO_B - 1; // 3
constexpr int MIN_CLAVES_B = GRADO_MINIMO_B - 1;     // 1

struct NodoB {
    std::vector<Cliente> claves;   // ordenadas por Cliente::id, hasta MAX_CLAVES_B
    std::vector<NodoB*> hijos;     // hasta MAX_CLAVES_B + 1, vacio si es hoja
    bool esHoja;

    explicit NodoB(bool hoja) : esHoja(hoja) {}
};

// Arbol B de orden 4 para administrar clientes, indexado por "id".
// Implementacion 100% manual (sin std::map/std::set ni ninguna
// estructura de arbol de la STL), tal como exige el enunciado.
class ArbolBClientes {
private:
    NodoB* raiz;
    int tamanio;

    // ---- busqueda ----
    // Retorna el nodo donde vive la clave y el indice dentro de ese
    // nodo, o nullptr si no existe.
    NodoB* buscarRec(NodoB* nodo, const std::string& id, int& indiceOut) const;

    // ---- insercion (CLRS 18.2) ----
    void dividirHijo(NodoB* padre, int indiceHijo);
    void insertarNoLleno(NodoB* nodo, const Cliente& c);

    // ---- eliminacion (CLRS 18.3) ----
    void eliminarRec(NodoB* nodo, const std::string& id);
    int encontrarIndiceClave(NodoB* nodo, const std::string& id) const;
    Cliente obtenerPredecesor(NodoB* nodo, int indice) const;
    Cliente obtenerSucesor(NodoB* nodo, int indice) const;
    void rellenarHijo(NodoB* nodo, int indiceHijo);
    void prestarDeAnterior(NodoB* nodo, int indiceHijo);
    void prestarDeSiguiente(NodoB* nodo, int indiceHijo);
    void fusionarConSiguiente(NodoB* nodo, int indiceHijo);

    void liberar(NodoB* nodo);
    void inordenRec(NodoB* nodo, std::vector<Cliente>& salida) const;

public:
    ArbolBClientes();
    ~ArbolBClientes();

    ArbolBClientes(const ArbolBClientes&) = delete;
    ArbolBClientes& operator=(const ArbolBClientes&) = delete;

    // Inserta un cliente nuevo. Retorna false si el id ya existe.
    bool insertar(const Cliente& c);

    // Elimina el cliente con ese id, aplicando redistribucion/fusion
    // de nodos segun corresponda. NO elimina las reservas asociadas
    // en la Tabla Hash -- esa eliminacion en cascada es orquestada por
    // quien llama (el panel de administrador), igual que con el AVL.
    bool eliminar(const std::string& id);

    // Edita los datos de un cliente existente sin mover el nodo (el
    // id, que es la clave, no cambia via este metodo).
    bool editar(const std::string& id, const Cliente& datosNuevos);

    Cliente* buscar(const std::string& id);
    bool existe(const std::string& id) const;

    int getTamanio() const;
    bool estaVacio() const;

    // "Listar todos los clientes ordenados por ID" (enunciado 4.1,
    // punto 05): equivalente al inorden de un BST, pero visitando
    // varias claves por nodo.
    std::vector<Cliente> listarClientes() const;

    NodoB* getRaiz() const; // usado por ReportesGraphviz
};

#endif // ARBOL_B_CLIENTES_H
