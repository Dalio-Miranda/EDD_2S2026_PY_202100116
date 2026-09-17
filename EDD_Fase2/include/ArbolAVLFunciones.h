#ifndef ARBOL_AVL_FUNCIONES_H
#define ARBOL_AVL_FUNCIONES_H

#include <string>
#include "ArregloDinamico.h"
#include "TipoRecorrido.h"
#include "Funcion.h"

struct NodoAVL {
    Funcion dato;
    NodoAVL* izquierdo;
    NodoAVL* derecho;
    int altura;

    explicit NodoAVL(const Funcion& f)
        : dato(f), izquierdo(nullptr), derecho(nullptr), altura(1) {}
};

// Arbol AVL que administra las funciones de cine, indexado por
// codigo_funcion. Implementacion 100% manual: solo usa punteros
// propios (izquierdo/derecho), sin ningun contenedor de la STL. Los
// recorridos devuelven un ArregloDinamico (arreglo dinamico propio,
// ver ArregloDinamico.h) en vez de std::vector.
class ArbolAVLFunciones {
private:
    NodoAVL* raiz;
    int tamanio;

    int altura(NodoAVL* nodo) const;
    int factorBalance(NodoAVL* nodo) const;
    void actualizarAltura(NodoAVL* nodo);

    NodoAVL* rotacionDerecha(NodoAVL* y);
    NodoAVL* rotacionIzquierda(NodoAVL* x);
    NodoAVL* balancear(NodoAVL* nodo);

    NodoAVL* insertarRec(NodoAVL* nodo, const Funcion& f, bool& insertado);
    NodoAVL* eliminarRec(NodoAVL* nodo, const std::string& codigo, bool& eliminado);
    NodoAVL* encontrarMinimo(NodoAVL* nodo) const;

    NodoAVL* buscarRec(NodoAVL* nodo, const std::string& codigo) const;

    void liberar(NodoAVL* nodo);

    void preordenRec(NodoAVL* nodo, ArregloDinamico<Funcion>& salida) const;
    void inordenRec(NodoAVL* nodo, ArregloDinamico<Funcion>& salida) const;
    void postordenRec(NodoAVL* nodo, ArregloDinamico<Funcion>& salida) const;

public:
    ArbolAVLFunciones();
    ~ArbolAVLFunciones();

    ArbolAVLFunciones(const ArbolAVLFunciones&) = delete;
    ArbolAVLFunciones& operator=(const ArbolAVLFunciones&) = delete;

    bool insertar(const Funcion& f);
    bool eliminar(const std::string& codigoFuncion);
    bool editar(const std::string& codigoFuncion, const Funcion& datosNuevos);

    Funcion* buscar(const std::string& codigoFuncion);
    bool existe(const std::string& codigoFuncion) const;

    int getTamanio() const;
    bool estaVacio() const;

    ArregloDinamico<Funcion> listarFunciones(TipoRecorrido tipo) const;

    NodoAVL* getRaiz() const;
};

#endif // ARBOL_AVL_FUNCIONES_H
