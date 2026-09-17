#ifndef ARBOL_BINARIO_PELICULAS_H
#define ARBOL_BINARIO_PELICULAS_H

#include <string>
#include "Pelicula.h"
#include "ArregloDinamico.h"
#include "TipoRecorrido.h"

// Nodo del arbol binario de busqueda (BST).
// Cada nodo tiene un hijo izquierdo (codigos menores) y un hijo
// derecho (codigos mayores), manteniendo la propiedad de orden
// del BST en todo momento.
struct NodoArbolPelicula {
    Pelicula dato;
    NodoArbolPelicula* izquierdo;
    NodoArbolPelicula* derecho;

    NodoArbolPelicula(const Pelicula& p)
        : dato(p), izquierdo(nullptr), derecho(nullptr) {}
};

// Arbol Binario de Busqueda que organiza la cartelera de peliculas
// por codigo. El recorrido INORDEN (izquierda, raiz, derecha)
// entrega las peliculas ordenadas ascendentemente por codigo,
// que es justo lo que pide el enunciado para mostrar la cartelera.
//
// Justificacion para el manual tecnico:
// Se eligio BST porque el enunciado lo especifica directamente,
// y porque un BST balanceado da complejidad O(log n) tanto para
// insertar como para buscar, muy superior a una lista enlazada
// (O(n)) cuando la cartelera crece. La busqueda por codigo se
// resuelve de forma natural recorriendo el arbol (izquierda si el
// codigo buscado es menor, derecha si es mayor), sin necesidad de
// un algoritmo de busqueda aparte: el propio BST ya es una forma
// de busqueda binaria.
class ArbolBinarioPeliculas {
private:
    NodoArbolPelicula* raiz;
    int tamanio;

public:
    ArbolBinarioPeliculas();
    ~ArbolBinarioPeliculas();

    // Retorna true si se inserto un codigo nuevo; false si el codigo
    // ya existia (en ese caso ACTUALIZA los datos en vez de duplicar).
    bool insertar(const Pelicula& p);
    bool eliminar(const std::string& codigo);
    Pelicula* buscar(const std::string& codigo);

    int cargarDesdeCSV(const std::string& rutaArchivo);

    // Recorrido inorden: imprime la cartelera ordenada por codigo.
    void imprimirInorden() const;

    // Listado con el recorrido solicitado (enunciado 4.2, "Gestionar
    // peliculas": preorden, inorden y postorden).
    ArregloDinamico<Pelicula> listarPeliculas(TipoRecorrido tipo) const;

    int getTamanio() const;
    bool estaVacia() const;

    // Acceso a la raiz, util para el reporte de Graphviz mas adelante.
    NodoArbolPelicula* getRaiz() const;

private:
    NodoArbolPelicula* insertarRec(NodoArbolPelicula* nodo, const Pelicula& p, bool& insertado);
    NodoArbolPelicula* eliminarRec(NodoArbolPelicula* nodo, const std::string& codigo, bool& eliminado);
    NodoArbolPelicula* encontrarMinimo(NodoArbolPelicula* nodo) const;
    Pelicula* buscarRec(NodoArbolPelicula* nodo, const std::string& codigo) const;
    void imprimirInordenRec(NodoArbolPelicula* nodo) const;
    void liberar(NodoArbolPelicula* nodo);

    void preordenRec(NodoArbolPelicula* nodo, ArregloDinamico<Pelicula>& salida) const;
    void inordenRec(NodoArbolPelicula* nodo, ArregloDinamico<Pelicula>& salida) const;
    void postordenRec(NodoArbolPelicula* nodo, ArregloDinamico<Pelicula>& salida) const;
};

#endif // ARBOL_BINARIO_PELICULAS_H
