#include "ArbolAVLFunciones.h"
#include <algorithm>

ArbolAVLFunciones::ArbolAVLFunciones() : raiz(nullptr), tamanio(0) {}

ArbolAVLFunciones::~ArbolAVLFunciones() {
    liberar(raiz);
}

void ArbolAVLFunciones::liberar(NodoAVL* nodo) {
    if (nodo == nullptr) return;
    liberar(nodo->izquierdo);
    liberar(nodo->derecho);
    delete nodo;
}

int ArbolAVLFunciones::altura(NodoAVL* nodo) const {
    return nodo == nullptr ? 0 : nodo->altura;
}

int ArbolAVLFunciones::factorBalance(NodoAVL* nodo) const {
    if (nodo == nullptr) return 0;
    return altura(nodo->izquierdo) - altura(nodo->derecho);
}

void ArbolAVLFunciones::actualizarAltura(NodoAVL* nodo) {
    nodo->altura = 1 + std::max(altura(nodo->izquierdo), altura(nodo->derecho));
}

NodoAVL* ArbolAVLFunciones::rotacionDerecha(NodoAVL* y) {
    NodoAVL* x = y->izquierdo;
    NodoAVL* subarbolT2 = x->derecho;

    x->derecho = y;
    y->izquierdo = subarbolT2;

    actualizarAltura(y);
    actualizarAltura(x);

    return x;
}

NodoAVL* ArbolAVLFunciones::rotacionIzquierda(NodoAVL* x) {
    NodoAVL* y = x->derecho;
    NodoAVL* subarbolT2 = y->izquierdo;

    y->izquierdo = x;
    x->derecho = subarbolT2;

    actualizarAltura(x);
    actualizarAltura(y);

    return y;
}

NodoAVL* ArbolAVLFunciones::balancear(NodoAVL* nodo) {
    actualizarAltura(nodo);
    int balance = factorBalance(nodo);

    if (balance > 1 && factorBalance(nodo->izquierdo) >= 0) {
        return rotacionDerecha(nodo);
    }
    if (balance > 1 && factorBalance(nodo->izquierdo) < 0) {
        nodo->izquierdo = rotacionIzquierda(nodo->izquierdo);
        return rotacionDerecha(nodo);
    }
    if (balance < -1 && factorBalance(nodo->derecho) <= 0) {
        return rotacionIzquierda(nodo);
    }
    if (balance < -1 && factorBalance(nodo->derecho) > 0) {
        nodo->derecho = rotacionDerecha(nodo->derecho);
        return rotacionIzquierda(nodo);
    }

    return nodo;
}

NodoAVL* ArbolAVLFunciones::insertarRec(NodoAVL* nodo, const Funcion& f, bool& insertado) {
    if (nodo == nullptr) {
        insertado = true;
        return new NodoAVL(f);
    }

    if (f.codigoFuncion < nodo->dato.codigoFuncion) {
        nodo->izquierdo = insertarRec(nodo->izquierdo, f, insertado);
    } else if (f.codigoFuncion > nodo->dato.codigoFuncion) {
        nodo->derecho = insertarRec(nodo->derecho, f, insertado);
    } else {
        insertado = false;
        return nodo;
    }

    return balancear(nodo);
}

bool ArbolAVLFunciones::insertar(const Funcion& f) {
    bool insertado = false;
    raiz = insertarRec(raiz, f, insertado);
    if (insertado) tamanio++;
    return insertado;
}

NodoAVL* ArbolAVLFunciones::encontrarMinimo(NodoAVL* nodo) const {
    NodoAVL* actual = nodo;
    while (actual->izquierdo != nullptr) {
        actual = actual->izquierdo;
    }
    return actual;
}

NodoAVL* ArbolAVLFunciones::eliminarRec(NodoAVL* nodo, const std::string& codigo, bool& eliminado) {
    if (nodo == nullptr) {
        eliminado = false;
        return nullptr;
    }

    if (codigo < nodo->dato.codigoFuncion) {
        nodo->izquierdo = eliminarRec(nodo->izquierdo, codigo, eliminado);
    } else if (codigo > nodo->dato.codigoFuncion) {
        nodo->derecho = eliminarRec(nodo->derecho, codigo, eliminado);
    } else {
        if (nodo->izquierdo == nullptr || nodo->derecho == nullptr) {
            NodoAVL* hijo = (nodo->izquierdo != nullptr) ? nodo->izquierdo : nodo->derecho;
            delete nodo;
            eliminado = true;
            return hijo;
        }

        NodoAVL* sucesor = encontrarMinimo(nodo->derecho);
        nodo->dato = sucesor->dato;

        bool eliminadoSucesor = false;
        nodo->derecho = eliminarRec(nodo->derecho, sucesor->dato.codigoFuncion, eliminadoSucesor);
        eliminado = true;
    }

    if (nodo == nullptr) return nullptr;
    return balancear(nodo);
}

bool ArbolAVLFunciones::eliminar(const std::string& codigoFuncion) {
    bool eliminado = false;
    raiz = eliminarRec(raiz, codigoFuncion, eliminado);
    if (eliminado) tamanio--;
    return eliminado;
}

bool ArbolAVLFunciones::editar(const std::string& codigoFuncion, const Funcion& datosNuevos) {
    NodoAVL* nodo = buscarRec(raiz, codigoFuncion);
    if (nodo == nullptr) return false;

    Funcion actualizado = datosNuevos;
    actualizado.codigoFuncion = codigoFuncion;
    nodo->dato = actualizado;
    return true;
}

NodoAVL* ArbolAVLFunciones::buscarRec(NodoAVL* nodo, const std::string& codigo) const {
    if (nodo == nullptr) return nullptr;
    if (codigo == nodo->dato.codigoFuncion) return nodo;
    if (codigo < nodo->dato.codigoFuncion) return buscarRec(nodo->izquierdo, codigo);
    return buscarRec(nodo->derecho, codigo);
}

Funcion* ArbolAVLFunciones::buscar(const std::string& codigoFuncion) {
    NodoAVL* nodo = buscarRec(raiz, codigoFuncion);
    return nodo == nullptr ? nullptr : &nodo->dato;
}

bool ArbolAVLFunciones::existe(const std::string& codigoFuncion) const {
    return buscarRec(raiz, codigoFuncion) != nullptr;
}

int ArbolAVLFunciones::getTamanio() const { return tamanio; }
bool ArbolAVLFunciones::estaVacio() const { return raiz == nullptr; }
NodoAVL* ArbolAVLFunciones::getRaiz() const { return raiz; }

void ArbolAVLFunciones::preordenRec(NodoAVL* nodo, ArregloDinamico<Funcion>& salida) const {
    if (nodo == nullptr) return;
    salida.agregarFinal(nodo->dato);
    preordenRec(nodo->izquierdo, salida);
    preordenRec(nodo->derecho, salida);
}

void ArbolAVLFunciones::inordenRec(NodoAVL* nodo, ArregloDinamico<Funcion>& salida) const {
    if (nodo == nullptr) return;
    inordenRec(nodo->izquierdo, salida);
    salida.agregarFinal(nodo->dato);
    inordenRec(nodo->derecho, salida);
}

void ArbolAVLFunciones::postordenRec(NodoAVL* nodo, ArregloDinamico<Funcion>& salida) const {
    if (nodo == nullptr) return;
    postordenRec(nodo->izquierdo, salida);
    postordenRec(nodo->derecho, salida);
    salida.agregarFinal(nodo->dato);
}

ArregloDinamico<Funcion> ArbolAVLFunciones::listarFunciones(TipoRecorrido tipo) const {
    ArregloDinamico<Funcion> salida;

    switch (tipo) {
        case TipoRecorrido::PREORDEN:
            preordenRec(raiz, salida);
            break;
        case TipoRecorrido::INORDEN:
            inordenRec(raiz, salida);
            break;
        case TipoRecorrido::POSTORDEN:
            postordenRec(raiz, salida);
            break;
    }

    return salida;
}
