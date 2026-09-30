#include "ArbolMerkle.h"
#include "SHA256.h"

ArbolMerkle::~ArbolMerkle() {
    liberar(raiz);
}

void ArbolMerkle::liberar(NodoMerkle* nodo) {
    if (nodo == nullptr) return;
    liberar(nodo->izquierdo);
    liberar(nodo->derecho);
    delete nodo;
}

void ArbolMerkle::construir(const ArregloDinamico<Transaccion>& transacciones) {
    liberar(raiz);
    raiz = nullptr;

    if (transacciones.estaVacio()) {
        // Bloque sin transacciones (ej. el genesis): la raiz es el
        // hash de un texto vacio, un valor fijo y reproducible.
        raiz = new NodoMerkle();
        raiz->hash = SHA256::calcular("");
        raiz->esHoja = true;
        return;
    }

    // Nivel 0: una hoja por transaccion.
    ArregloDinamico<NodoMerkle*> nivelActual;
    for (int i = 0; i < transacciones.getTamanio(); i++) {
        NodoMerkle* hoja = new NodoMerkle();
        hoja->esHoja = true;
        hoja->codigoTransaccion = transacciones[i].codigoTransaccion;
        hoja->hash = SHA256::calcular(transacciones[i].aTexto());
        nivelActual.agregarFinal(hoja);
    }

    // Subir de nivel en nivel, combinando de a pares, hasta que solo
    // quede 1 nodo (la raiz).
    while (nivelActual.getTamanio() > 1) {
        ArregloDinamico<NodoMerkle*> nivelSiguiente;

        for (int i = 0; i < nivelActual.getTamanio(); i += 2) {
            NodoMerkle* izq = nivelActual[i];
            NodoMerkle* der;

            if (i + 1 < nivelActual.getTamanio()) {
                der = nivelActual[i + 1];
            } else {
                // Cantidad impar de nodos en este nivel: se duplica
                // el ultimo (convencion estandar de los arboles de
                // Merkle). IMPORTANTE: se crea un nodo NUEVO con el
                // mismo hash, nunca se reutiliza el mismo puntero dos
                // veces -- si no, al liberar el arbol se intentaria
                // borrar el mismo nodo dos veces (double free).
                der = new NodoMerkle();
                der->hash = izq->hash;
                der->esHoja = izq->esHoja;
                der->esDuplicado = true;
                der->codigoTransaccion = izq->codigoTransaccion;
            }

            NodoMerkle* padre = new NodoMerkle();
            padre->esHoja = false;
            padre->izquierdo = izq;
            padre->derecho = der;
            padre->hash = SHA256::calcular(izq->hash + der->hash);

            nivelSiguiente.agregarFinal(padre);
        }

        nivelActual = nivelSiguiente;
    }

    raiz = nivelActual[0];
}

std::string ArbolMerkle::raizHash() const {
    return raiz == nullptr ? "" : raiz->hash;
}

NodoMerkle* ArbolMerkle::getRaiz() const {
    return raiz;
}

std::string ArbolMerkle::calcularRaiz(const ArregloDinamico<Transaccion>& transacciones) {
    ArbolMerkle temporal;
    temporal.construir(transacciones);
    return temporal.raizHash();
}
