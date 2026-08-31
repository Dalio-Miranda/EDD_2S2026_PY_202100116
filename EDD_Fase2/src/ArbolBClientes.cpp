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
    while (i < static_cast<int>(nodo->claves.size()) && id > nodo->claves[i].id) {
        i++;
    }

    if (i < static_cast<int>(nodo->claves.size()) && id == nodo->claves[i].id) {
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

// Divide al hijo lleno nodo->hijos[indiceHijo] (que tiene exactamente
// MAX_CLAVES_B claves) en dos, y sube su clave mediana a 'nodo'.
void ArbolBClientes::dividirHijo(NodoB* nodo, int indiceHijo) {
    NodoB* hijoLleno = nodo->hijos[indiceHijo];
    NodoB* nuevoHijo = new NodoB(hijoLleno->esHoja);

    // Las ultimas (GRADO_MINIMO_B - 1) claves de hijoLleno pasan al
    // nuevo nodo hermano.
    for (int j = 0; j < GRADO_MINIMO_B - 1; j++) {
        nuevoHijo->claves.push_back(hijoLleno->claves[j + GRADO_MINIMO_B]);
    }

    // Si no es hoja, tambien se reparten los hijos correspondientes.
    if (!hijoLleno->esHoja) {
        for (int j = 0; j < GRADO_MINIMO_B; j++) {
            nuevoHijo->hijos.push_back(hijoLleno->hijos[j + GRADO_MINIMO_B]);
        }
        hijoLleno->hijos.resize(GRADO_MINIMO_B);
    }

    // La clave mediana (indice GRADO_MINIMO_B - 1) sube al padre.
    Cliente medianaSubida = hijoLleno->claves[GRADO_MINIMO_B - 1];
    hijoLleno->claves.resize(GRADO_MINIMO_B - 1);

    // Se inserta el puntero al nuevo hijo y la clave mediana en 'nodo'.
    nodo->hijos.insert(nodo->hijos.begin() + indiceHijo + 1, nuevoHijo);
    nodo->claves.insert(nodo->claves.begin() + indiceHijo, medianaSubida);
}

void ArbolBClientes::insertarNoLleno(NodoB* nodo, const Cliente& c) {
    int i = static_cast<int>(nodo->claves.size()) - 1;

    if (nodo->esHoja) {
        // Insertar en la posicion ordenada correcta dentro de la hoja.
        nodo->claves.push_back(c); // placeholder, se reordena abajo
        while (i >= 0 && c.id < nodo->claves[i].id) {
            nodo->claves[i + 1] = nodo->claves[i];
            i--;
        }
        nodo->claves[i + 1] = c;
    } else {
        while (i >= 0 && c.id < nodo->claves[i].id) {
            i--;
        }
        i++; // indice del hijo donde debe descender

        if (static_cast<int>(nodo->hijos[i]->claves.size()) == MAX_CLAVES_B) {
            dividirHijo(nodo, i);
            if (c.id > nodo->claves[i].id) {
                i++;
            }
        }
        insertarNoLleno(nodo->hijos[i], c);
    }
}

bool ArbolBClientes::insertar(const Cliente& c) {
    if (existe(c.id)) return false; // id duplicado: no se inserta

    if (raiz == nullptr) {
        raiz = new NodoB(true);
        raiz->claves.push_back(c);
    } else if (static_cast<int>(raiz->claves.size()) == MAX_CLAVES_B) {
        // La raiz esta llena: crece el arbol un nivel.
        NodoB* nuevaRaiz = new NodoB(false);
        nuevaRaiz->hijos.push_back(raiz);
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
    actualizado.id = id; // la clave no cambia
    nodo->claves[indice] = actualizado;
    return true;
}

// ------------------------------- eliminacion --------------------------------
// Sigue el algoritmo estandar de eliminacion de arboles B (CLRS 18.3).

int ArbolBClientes::encontrarIndiceClave(NodoB* nodo, const std::string& id) const {
    int i = 0;
    while (i < static_cast<int>(nodo->claves.size()) && nodo->claves[i].id < id) {
        i++;
    }
    return i;
}

Cliente ArbolBClientes::obtenerPredecesor(NodoB* nodo, int indice) const {
    // El predecesor es el maximo del subarbol izquierdo de claves[indice].
    NodoB* actual = nodo->hijos[indice];
    while (!actual->esHoja) {
        actual = actual->hijos.back();
    }
    return actual->claves.back();
}

Cliente ArbolBClientes::obtenerSucesor(NodoB* nodo, int indice) const {
    // El sucesor es el minimo del subarbol derecho de claves[indice].
    NodoB* actual = nodo->hijos[indice + 1];
    while (!actual->esHoja) {
        actual = actual->hijos.front();
    }
    return actual->claves.front();
}

// Toma prestada una clave del hermano IZQUIERDO de hijos[indiceHijo],
// rotando a traves del padre.
void ArbolBClientes::prestarDeAnterior(NodoB* nodo, int indiceHijo) {
    NodoB* hijo = nodo->hijos[indiceHijo];
    NodoB* hermano = nodo->hijos[indiceHijo - 1];

    // La clave del padre baja al frente del hijo actual.
    hijo->claves.insert(hijo->claves.begin(), nodo->claves[indiceHijo - 1]);

    // La ultima clave del hermano sube al padre.
    nodo->claves[indiceHijo - 1] = hermano->claves.back();
    hermano->claves.pop_back();

    // Si no son hojas, tambien se mueve el ultimo hijo del hermano.
    if (!hijo->esHoja) {
        hijo->hijos.insert(hijo->hijos.begin(), hermano->hijos.back());
        hermano->hijos.pop_back();
    }
}

// Toma prestada una clave del hermano DERECHO de hijos[indiceHijo].
void ArbolBClientes::prestarDeSiguiente(NodoB* nodo, int indiceHijo) {
    NodoB* hijo = nodo->hijos[indiceHijo];
    NodoB* hermano = nodo->hijos[indiceHijo + 1];

    // La clave del padre baja al final del hijo actual.
    hijo->claves.push_back(nodo->claves[indiceHijo]);

    // La primera clave del hermano sube al padre.
    nodo->claves[indiceHijo] = hermano->claves.front();
    hermano->claves.erase(hermano->claves.begin());

    // Si no son hojas, tambien se mueve el primer hijo del hermano.
    if (!hijo->esHoja) {
        hijo->hijos.push_back(hermano->hijos.front());
        hermano->hijos.erase(hermano->hijos.begin());
    }
}

// Fusiona hijos[indiceHijo], la clave nodo->claves[indiceHijo] y
// hijos[indiceHijo + 1] en un solo nodo (hijos[indiceHijo]), y libera
// el hermano derecho.
void ArbolBClientes::fusionarConSiguiente(NodoB* nodo, int indiceHijo) {
    NodoB* hijo = nodo->hijos[indiceHijo];
    NodoB* hermano = nodo->hijos[indiceHijo + 1];

    // Baja la clave del padre al hijo, seguida de todas las claves
    // del hermano.
    hijo->claves.push_back(nodo->claves[indiceHijo]);
    for (const Cliente& c : hermano->claves) {
        hijo->claves.push_back(c);
    }
    if (!hijo->esHoja) {
        for (NodoB* h : hermano->hijos) {
            hijo->hijos.push_back(h);
        }
    }

    // Elimina del padre la clave y el puntero al hermano fusionado.
    nodo->claves.erase(nodo->claves.begin() + indiceHijo);
    nodo->hijos.erase(nodo->hijos.begin() + indiceHijo + 1);

    delete hermano;
}

// Garantiza que hijos[indiceHijo] tenga al menos GRADO_MINIMO_B claves
// antes de descender a el, pidiendo prestado a un hermano o fusionando.
void ArbolBClientes::rellenarHijo(NodoB* nodo, int indiceHijo) {
    if (indiceHijo != 0 &&
        static_cast<int>(nodo->hijos[indiceHijo - 1]->claves.size()) >= GRADO_MINIMO_B) {
        prestarDeAnterior(nodo, indiceHijo);
    } else if (indiceHijo != static_cast<int>(nodo->hijos.size()) - 1 &&
               static_cast<int>(nodo->hijos[indiceHijo + 1]->claves.size()) >= GRADO_MINIMO_B) {
        prestarDeSiguiente(nodo, indiceHijo);
    } else {
        // Ambos hermanos estan en el minimo: fusionar.
        if (indiceHijo != static_cast<int>(nodo->hijos.size()) - 1) {
            fusionarConSiguiente(nodo, indiceHijo);
        } else {
            fusionarConSiguiente(nodo, indiceHijo - 1);
        }
    }
}

void ArbolBClientes::eliminarRec(NodoB* nodo, const std::string& id) {
    int indice = encontrarIndiceClave(nodo, id);

    if (indice < static_cast<int>(nodo->claves.size()) && nodo->claves[indice].id == id) {
        // La clave esta en este nodo.
        if (nodo->esHoja) {
            // Caso 1: nodo hoja, se elimina directamente.
            nodo->claves.erase(nodo->claves.begin() + indice);
        } else {
            // Caso 2: nodo interno.
            if (static_cast<int>(nodo->hijos[indice]->claves.size()) >= GRADO_MINIMO_B) {
                // 2a: el hijo predecesor tiene suficientes claves.
                Cliente pred = obtenerPredecesor(nodo, indice);
                nodo->claves[indice] = pred;
                eliminarRec(nodo->hijos[indice], pred.id);
            } else if (static_cast<int>(nodo->hijos[indice + 1]->claves.size()) >= GRADO_MINIMO_B) {
                // 2b: el hijo sucesor tiene suficientes claves.
                Cliente suc = obtenerSucesor(nodo, indice);
                nodo->claves[indice] = suc;
                eliminarRec(nodo->hijos[indice + 1], suc.id);
            } else {
                // 2c: ambos hijos tienen el minimo -> fusionar y
                // eliminar recursivamente desde el nodo fusionado.
                fusionarConSiguiente(nodo, indice);
                eliminarRec(nodo->hijos[indice], id);
            }
        }
    } else {
        // La clave no esta en este nodo: debe estar (si existe) en
        // el subarbol hijos[indice].
        if (nodo->esHoja) {
            return; // no existe en el arbol
        }

        bool esUltimoHijo = (indice == static_cast<int>(nodo->hijos.size()) - 1);

        if (static_cast<int>(nodo->hijos[indice]->claves.size()) < GRADO_MINIMO_B) {
            rellenarHijo(nodo, indice);
        }

        // Si se fusiono con el ultimo hijo, el indice a seguir cambia.
        if (esUltimoHijo && indice > static_cast<int>(nodo->claves.size())) {
            eliminarRec(nodo->hijos[indice - 1], id);
        } else {
            eliminarRec(nodo->hijos[indice], id);
        }
    }
}

bool ArbolBClientes::eliminar(const std::string& id) {
    if (raiz == nullptr || !existe(id)) return false;

    eliminarRec(raiz, id);

    // Si la raiz se quedo sin claves y tiene un hijo, ese hijo pasa a
    // ser la nueva raiz (el arbol se encoge un nivel).
    if (raiz->claves.empty() && !raiz->esHoja) {
        NodoB* raizVieja = raiz;
        raiz = raiz->hijos[0];
        raizVieja->hijos.clear(); // evitar que liberar() borre el nuevo raiz
        delete raizVieja;
    } else if (raiz->claves.empty() && raiz->esHoja) {
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

void ArbolBClientes::inordenRec(NodoB* nodo, std::vector<Cliente>& salida) const {
    if (nodo == nullptr) return;

    int i;
    for (i = 0; i < static_cast<int>(nodo->claves.size()); i++) {
        if (!nodo->esHoja) {
            inordenRec(nodo->hijos[i], salida);
        }
        salida.push_back(nodo->claves[i]);
    }
    if (!nodo->esHoja) {
        inordenRec(nodo->hijos[i], salida);
    }
}

std::vector<Cliente> ArbolBClientes::listarClientes() const {
    std::vector<Cliente> salida;
    salida.reserve(tamanio);
    inordenRec(raiz, salida);
    return salida;
}
