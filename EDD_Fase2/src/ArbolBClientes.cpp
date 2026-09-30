#include "ArbolBClientes.h"

ArbolBClientes::ArbolBClientes() : raiz(nullptr), tamanio(0) {}

ArbolBClientes::~ArbolBClientes() {
    liberar(raiz);
}

void ArbolBClientes::liberar(NodoB* nodo) {
    if (nodo == nullptr) return;
    if (!nodo->esHoja) {
        for (NodoB* hijo : nodo->hijos) {
            liberar(hijo);
        }
    }
    delete nodo;
}

// ------------------------------- busqueda --------------------------------

NodoB* ArbolBClientes::buscarRec(NodoB* nodo, const std::string& id, int& indiceOut) const {
    if (nodo == nullptr) return nullptr;

    int i = 0;
    while (i < nodo->claves.getTamanio() && id > nodo->claves[i].id) {
        i++;
    }

    if (i < nodo->claves.getTamanio() && id == nodo->claves[i].id) {
        indiceOut = i;
        return nodo;
    }

    if (nodo->esHoja) return nullptr;

    return buscarRec(nodo->hijos[i], id, indiceOut);
}

Cliente* ArbolBClientes::buscar(const std::string& id) {
    int indice = -1;
    NodoB* nodo = buscarRec(raiz, id, indice);
    if (nodo == nullptr) return nullptr;
    return &nodo->claves[indice];
}

bool ArbolBClientes::existe(const std::string& id) const {
    int indice = -1;
    return buscarRec(raiz, id, indice) != nullptr;
}

// ------------------------------- insercion --------------------------------

void ArbolBClientes::dividirHijo(NodoB* nodo, int indiceHijo) {
    NodoB* hijoLleno = nodo->hijos[indiceHijo];
    NodoB* nuevoHijo = new NodoB(hijoLleno->esHoja);

    for (int j = 0; j < GRADO_MINIMO_B - 1; j++) {
        nuevoHijo->claves.agregarFinal(hijoLleno->claves[j + GRADO_MINIMO_B]);
    }

    if (!hijoLleno->esHoja) {
        for (int j = 0; j < GRADO_MINIMO_B; j++) {
            nuevoHijo->hijos.agregarFinal(hijoLleno->hijos[j + GRADO_MINIMO_B]);
        }
        hijoLleno->hijos.ajustarTamanio(GRADO_MINIMO_B);
    }

    Cliente medianaSubida = hijoLleno->claves[GRADO_MINIMO_B - 1];
    hijoLleno->claves.ajustarTamanio(GRADO_MINIMO_B - 1);

    nodo->hijos.insertarEn(indiceHijo + 1, nuevoHijo);
    nodo->claves.insertarEn(indiceHijo, medianaSubida);
}

void ArbolBClientes::insertarNoLleno(NodoB* nodo, const Cliente& c) {
    int i = nodo->claves.getTamanio() - 1;

    if (nodo->esHoja) {
        nodo->claves.agregarFinal(c); // placeholder, se reordena abajo
        while (i >= 0 && c.id < nodo->claves[i].id) {
            nodo->claves[i + 1] = nodo->claves[i];
            i--;
        }
        nodo->claves[i + 1] = c;
    } else {
        while (i >= 0 && c.id < nodo->claves[i].id) {
            i--;
        }
        i++;

        if (nodo->hijos[i]->claves.getTamanio() == MAX_CLAVES_B) {
            dividirHijo(nodo, i);
            if (c.id > nodo->claves[i].id) {
                i++;
            }
        }
        insertarNoLleno(nodo->hijos[i], c);
    }
}

bool ArbolBClientes::insertar(const Cliente& c) {
    if (existe(c.id)) return false;

    if (raiz == nullptr) {
        raiz = new NodoB(true);
        raiz->claves.agregarFinal(c);
    } else if (raiz->claves.getTamanio() == MAX_CLAVES_B) {
        NodoB* nuevaRaiz = new NodoB(false);
        nuevaRaiz->hijos.agregarFinal(raiz);
        dividirHijo(nuevaRaiz, 0);
        raiz = nuevaRaiz;
        insertarNoLleno(raiz, c);
    } else {
        insertarNoLleno(raiz, c);
    }

    tamanio++;
    return true;
}

// ------------------------------- edicion --------------------------------

bool ArbolBClientes::editar(const std::string& id, const Cliente& datosNuevos) {
    int indice = -1;
    NodoB* nodo = buscarRec(raiz, id, indice);
    if (nodo == nullptr) return false;

    Cliente actualizado = datosNuevos;
    actualizado.id = id;
    nodo->claves[indice] = actualizado;
    return true;
}

// ------------------------------- eliminacion --------------------------------

int ArbolBClientes::encontrarIndiceClave(NodoB* nodo, const std::string& id) const {
    int i = 0;
    while (i < nodo->claves.getTamanio() && nodo->claves[i].id < id) {
        i++;
    }
    return i;
}

Cliente ArbolBClientes::obtenerPredecesor(NodoB* nodo, int indice) const {
    NodoB* actual = nodo->hijos[indice];
    while (!actual->esHoja) {
        actual = actual->hijos.atras();
    }
    return actual->claves.atras();
}

Cliente ArbolBClientes::obtenerSucesor(NodoB* nodo, int indice) const {
    NodoB* actual = nodo->hijos[indice + 1];
    while (!actual->esHoja) {
        actual = actual->hijos.frente();
    }
    return actual->claves.frente();
}

void ArbolBClientes::prestarDeAnterior(NodoB* nodo, int indiceHijo) {
    NodoB* hijo = nodo->hijos[indiceHijo];
    NodoB* hermano = nodo->hijos[indiceHijo - 1];

    hijo->claves.insertarEn(0, nodo->claves[indiceHijo - 1]);

    nodo->claves[indiceHijo - 1] = hermano->claves.atras();
    hermano->claves.eliminarUltimo();

    if (!hijo->esHoja) {
        hijo->hijos.insertarEn(0, hermano->hijos.atras());
        hermano->hijos.eliminarUltimo();
    }
}

void ArbolBClientes::prestarDeSiguiente(NodoB* nodo, int indiceHijo) {
    NodoB* hijo = nodo->hijos[indiceHijo];
    NodoB* hermano = nodo->hijos[indiceHijo + 1];

    hijo->claves.agregarFinal(nodo->claves[indiceHijo]);

    nodo->claves[indiceHijo] = hermano->claves.frente();
    hermano->claves.eliminarEn(0);

    if (!hijo->esHoja) {
        hijo->hijos.agregarFinal(hermano->hijos.frente());
        hermano->hijos.eliminarEn(0);
    }
}

void ArbolBClientes::fusionarConSiguiente(NodoB* nodo, int indiceHijo) {
    NodoB* hijo = nodo->hijos[indiceHijo];
    NodoB* hermano = nodo->hijos[indiceHijo + 1];

    hijo->claves.agregarFinal(nodo->claves[indiceHijo]);
    for (const Cliente& c : hermano->claves) {
        hijo->claves.agregarFinal(c);
    }
    if (!hijo->esHoja) {
        for (NodoB* h : hermano->hijos) {
            hijo->hijos.agregarFinal(h);
        }
    }

    nodo->claves.eliminarEn(indiceHijo);
    nodo->hijos.eliminarEn(indiceHijo + 1);

    delete hermano;
}

void ArbolBClientes::rellenarHijo(NodoB* nodo, int indiceHijo) {
    if (indiceHijo != 0 &&
        nodo->hijos[indiceHijo - 1]->claves.getTamanio() >= GRADO_MINIMO_B) {
        prestarDeAnterior(nodo, indiceHijo);
    } else if (indiceHijo != nodo->hijos.getTamanio() - 1 &&
               nodo->hijos[indiceHijo + 1]->claves.getTamanio() >= GRADO_MINIMO_B) {
        prestarDeSiguiente(nodo, indiceHijo);
    } else {
        if (indiceHijo != nodo->hijos.getTamanio() - 1) {
            fusionarConSiguiente(nodo, indiceHijo);
        } else {
            fusionarConSiguiente(nodo, indiceHijo - 1);
        }
    }
}

void ArbolBClientes::eliminarRec(NodoB* nodo, const std::string& id) {
    int indice = encontrarIndiceClave(nodo, id);

    if (indice < nodo->claves.getTamanio() && nodo->claves[indice].id == id) {
        if (nodo->esHoja) {
            nodo->claves.eliminarEn(indice);
        } else {
            if (nodo->hijos[indice]->claves.getTamanio() >= GRADO_MINIMO_B) {
                Cliente pred = obtenerPredecesor(nodo, indice);
                nodo->claves[indice] = pred;
                eliminarRec(nodo->hijos[indice], pred.id);
            } else if (nodo->hijos[indice + 1]->claves.getTamanio() >= GRADO_MINIMO_B) {
                Cliente suc = obtenerSucesor(nodo, indice);
                nodo->claves[indice] = suc;
                eliminarRec(nodo->hijos[indice + 1], suc.id);
            } else {
                fusionarConSiguiente(nodo, indice);
                eliminarRec(nodo->hijos[indice], id);
            }
        }
    } else {
        if (nodo->esHoja) {
            return;
        }

        bool esUltimoHijo = (indice == nodo->hijos.getTamanio() - 1);

        if (nodo->hijos[indice]->claves.getTamanio() < GRADO_MINIMO_B) {
            rellenarHijo(nodo, indice);
        }

        if (esUltimoHijo && indice > nodo->claves.getTamanio()) {
            eliminarRec(nodo->hijos[indice - 1], id);
        } else {
            eliminarRec(nodo->hijos[indice], id);
        }
    }
}

bool ArbolBClientes::eliminar(const std::string& id) {
    if (raiz == nullptr || !existe(id)) return false;

    eliminarRec(raiz, id);

    if (raiz->claves.estaVacio() && !raiz->esHoja) {
        NodoB* raizVieja = raiz;
        raiz = raiz->hijos[0];
        raizVieja->hijos.vaciar();
        delete raizVieja;
    } else if (raiz->claves.estaVacio() && raiz->esHoja) {
        delete raiz;
        raiz = nullptr;
    }

    tamanio--;
    return true;
}

int ArbolBClientes::getTamanio() const { return tamanio; }
bool ArbolBClientes::estaVacio() const { return raiz == nullptr; }
NodoB* ArbolBClientes::getRaiz() const { return raiz; }

// ------------------------------- recorrido --------------------------------

void ArbolBClientes::inordenRec(NodoB* nodo, ArregloDinamico<Cliente>& salida) const {
    if (nodo == nullptr) return;

    int i;
    for (i = 0; i < nodo->claves.getTamanio(); i++) {
        if (!nodo->esHoja) {
            inordenRec(nodo->hijos[i], salida);
        }
        salida.agregarFinal(nodo->claves[i]);
    }
    if (!nodo->esHoja) {
        inordenRec(nodo->hijos[i], salida);
    }
}

void ArbolBClientes::preordenRec(NodoB* nodo, ArregloDinamico<Cliente>& salida) const {
    if (nodo == nullptr) return;

    // Primero TODAS las claves de este nodo (en orden), luego cada hijo.
    for (int i = 0; i < nodo->claves.getTamanio(); i++) {
        salida.agregarFinal(nodo->claves[i]);
    }
    if (!nodo->esHoja) {
        for (NodoB* hijo : nodo->hijos) {
            preordenRec(hijo, salida);
        }
    }
}

void ArbolBClientes::postordenRec(NodoB* nodo, ArregloDinamico<Cliente>& salida) const {
    if (nodo == nullptr) return;

    // Primero cada hijo, luego TODAS las claves de este nodo.
    if (!nodo->esHoja) {
        for (NodoB* hijo : nodo->hijos) {
            postordenRec(hijo, salida);
        }
    }
    for (int i = 0; i < nodo->claves.getTamanio(); i++) {
        salida.agregarFinal(nodo->claves[i]);
    }
}

ArregloDinamico<Cliente> ArbolBClientes::listarClientes(TipoRecorrido tipo) const {
    ArregloDinamico<Cliente> salida;

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
