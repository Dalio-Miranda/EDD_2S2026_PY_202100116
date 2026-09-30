#ifndef ARBOL_MERKLE_H
#define ARBOL_MERKLE_H

#include <string>
#include "ArregloDinamico.h"
#include "Transaccion.h"

// Nodo del Arbol de Merkle. Las hojas guardan el hash de UNA
// transaccion (hash = SHA256(transaccion.aTexto())); los nodos
// internos guardan el hash combinado de sus 2 hijos
// (hash = SHA256(hijoIzq.hash + hijoDer.hash)).
struct NodoMerkle {
    std::string hash;
    NodoMerkle* izquierdo = nullptr;
    NodoMerkle* derecho = nullptr;
    bool esHoja = false;

    // true si este nodo es una copia (mismo hash, sin hijos propios)
    // creada solo para poder combinar un nivel con cantidad impar de
    // nodos -- no representa una transaccion ni un hash "nuevo" real.
    bool esDuplicado = false;

    // Solo tiene sentido si esHoja == true: a que transaccion
    // corresponde esta hoja (para poder dibujarla en el reporte).
    std::string codigoTransaccion;
};

// Arbol de Merkle: un arbol binario en el que cada hoja es el hash de
// una transaccion, y cada nodo interno es el hash combinado de sus 2
// hijos. El nodo raiz (Merkle Root) es un resumen criptografico de
// TODO el conjunto de transacciones de un bloque: si UNA sola
// transaccion cambia (aunque sea un solo caracter), la raiz calculada
// cambia por completo, lo que permite detectar manipulacion.
//
// Si en algun nivel hay una cantidad IMPAR de nodos, el ultimo se
// duplica para poder seguir combinando de a pares (convencion
// estandar de los arboles de Merkle, ej. Bitcoin).
class ArbolMerkle {
private:
    NodoMerkle* raiz = nullptr;

    void liberar(NodoMerkle* nodo);

public:
    ArbolMerkle() = default;
    ~ArbolMerkle();

    ArbolMerkle(const ArbolMerkle&) = delete;
    ArbolMerkle& operator=(const ArbolMerkle&) = delete;

    // Construye el arbol completo a partir de las transacciones
    // dadas (libera cualquier arbol construido previamente). Si
    // "transacciones" esta vacio, la raiz sera el hash de un texto
    // vacio (SHA256("")).
    void construir(const ArregloDinamico<Transaccion>& transacciones);

    // El hash raiz (Merkle Root) del arbol ya construido. Cadena
    // vacia si construir() no se ha llamado todavia.
    std::string raizHash() const;

    NodoMerkle* getRaiz() const;

    // Atajo: calcula solo el hash raiz para un conjunto de
    // transacciones, sin necesidad de mantener el arbol completo en
    // memoria despues. Utilizado al minar un bloque nuevo (para
    // obtener ROOTMERKLE antes de la prueba de trabajo) y al
    // verificar la integridad de un bloque ya existente (recalcular
    // la raiz desde su DATA actual y compararla contra su
    // ROOTMERKLE almacenado).
    static std::string calcularRaiz(const ArregloDinamico<Transaccion>& transacciones);
};

#endif // ARBOL_MERKLE_H
