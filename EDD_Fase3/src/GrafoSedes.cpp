#include "GrafoSedes.h"

GrafoSedes::~GrafoSedes() {
    for (int i = 0; i < vertices.getTamanio(); i++) {
        delete vertices[i];
    }
}

NodoGrafoSede* GrafoSedes::buscarNodo(const std::string& codigo) const {
    for (int i = 0; i < vertices.getTamanio(); i++) {
        if (vertices[i]->dato.codigo == codigo) return vertices[i];
    }
    return nullptr;
}

int GrafoSedes::indiceDeArista(NodoGrafoSede* nodo, const std::string& codigoVecino) const {
    for (int i = 0; i < nodo->aristas.getTamanio(); i++) {
        if (nodo->aristas[i].sedeVecina == codigoVecino) return i;
    }
    return -1;
}

void GrafoSedes::establecerAristaUnidireccional(NodoGrafoSede* nodo, const std::string& codigoVecino, int peso) {
    int idx = indiceDeArista(nodo, codigoVecino);

    if (peso <= 0) {
        // Sin peliculas en comun: no debe existir arista.
        if (idx != -1) nodo->aristas.eliminarEn(idx);
        return;
    }

    if (idx != -1) {
        nodo->aristas[idx].peso = peso;
    } else {
        Arista a;
        a.sedeVecina = codigoVecino;
        a.peso = peso;
        nodo->aristas.agregarFinal(a);
    }
}

bool GrafoSedes::agregarSede(const Sede& s) {
    if (existeSede(s.codigo)) return false;
    vertices.agregarFinal(new NodoGrafoSede(s));
    return true;
}

Sede* GrafoSedes::buscarSede(const std::string& codigo) {
    NodoGrafoSede* nodo = buscarNodo(codigo);
    return nodo == nullptr ? nullptr : &nodo->dato;
}

bool GrafoSedes::existeSede(const std::string& codigo) const {
    return buscarNodo(codigo) != nullptr;
}

int GrafoSedes::getCantidadSedes() const {
    return vertices.getTamanio();
}

void GrafoSedes::establecerPeso(const std::string& codigoA, const std::string& codigoB, int peso) {
    if (codigoA == codigoB) return; // sin auto-relaciones

    NodoGrafoSede* nodoA = buscarNodo(codigoA);
    NodoGrafoSede* nodoB = buscarNodo(codigoB);
    if (nodoA == nullptr || nodoB == nullptr) return;

    // No dirigido: se actualiza la arista vista desde AMBOS extremos.
    establecerAristaUnidireccional(nodoA, codigoB, peso);
    establecerAristaUnidireccional(nodoB, codigoA, peso);
}

int GrafoSedes::getPeso(const std::string& codigoA, const std::string& codigoB) const {
    NodoGrafoSede* nodoA = buscarNodo(codigoA);
    if (nodoA == nullptr) return 0;

    int idx = indiceDeArista(nodoA, codigoB);
    return idx == -1 ? 0 : nodoA->aristas[idx].peso;
}

void GrafoSedes::recalcularRelacionesDeSede(const std::string& codigoSede, ArbolAVLFunciones& funciones) {
    if (!existeSede(codigoSede)) return;

    ArregloDinamico<Funcion> todasLasFunciones = funciones.listarFunciones(TipoRecorrido::INORDEN);

    // Catalogo (codigos de pelicula distintos) de la sede que cambio.
    ArregloDinamico<std::string> catalogoSede;
    for (int i = 0; i < todasLasFunciones.getTamanio(); i++) {
        const Funcion& f = todasLasFunciones[i];
        if (f.codigoSede == codigoSede && !catalogoSede.contiene(f.codigoPelicula)) {
            catalogoSede.agregarFinal(f.codigoPelicula);
        }
    }

    // Comparar contra el catalogo de CADA otra sede del grafo.
    for (int i = 0; i < vertices.getTamanio(); i++) {
        const std::string& codigoOtra = vertices[i]->dato.codigo;
        if (codigoOtra == codigoSede) continue;

        ArregloDinamico<std::string> catalogoOtra;
        for (int j = 0; j < todasLasFunciones.getTamanio(); j++) {
            const Funcion& f = todasLasFunciones[j];
            if (f.codigoSede == codigoOtra && !catalogoOtra.contiene(f.codigoPelicula)) {
                catalogoOtra.agregarFinal(f.codigoPelicula);
            }
        }

        int interseccion = 0;
        for (int k = 0; k < catalogoSede.getTamanio(); k++) {
            if (catalogoOtra.contiene(catalogoSede[k])) interseccion++;
        }

        establecerPeso(codigoSede, codigoOtra, interseccion);
    }
}

ArregloDinamico<std::string> GrafoSedes::caminoMasCorto(const std::string& origen, const std::string& destino) const {
    ArregloDinamico<std::string> resultado;

    if (!existeSede(origen) || !existeSede(destino)) return resultado;
    if (origen == destino) {
        resultado.agregarFinal(origen);
        return resultado;
    }

    // BFS manual: cola (ArregloDinamico + indice de frente, para
    // evitar el costo O(n) de eliminar siempre el primero), lista de
    // visitados, y lista paralela de predecesores para reconstruir
    // el camino al final.
    ArregloDinamico<std::string> cola;
    int frente = 0;
    ArregloDinamico<std::string> visitados;
    ArregloDinamico<std::string> predecesorDe;   // nodo
    ArregloDinamico<std::string> predecesorEs;   // su predecesor

    cola.agregarFinal(origen);
    visitados.agregarFinal(origen);

    bool encontrado = false;
    while (frente < cola.getTamanio() && !encontrado) {
        std::string actual = cola[frente];
        frente++;

        NodoGrafoSede* nodoActual = buscarNodo(actual);
        if (nodoActual == nullptr) continue;

        for (int i = 0; i < nodoActual->aristas.getTamanio(); i++) {
            const std::string& vecino = nodoActual->aristas[i].sedeVecina;
            if (visitados.contiene(vecino)) continue;

            visitados.agregarFinal(vecino);
            predecesorDe.agregarFinal(vecino);
            predecesorEs.agregarFinal(actual);
            cola.agregarFinal(vecino);

            if (vecino == destino) {
                encontrado = true;
                break;
            }
        }
    }

    if (!visitados.contiene(destino)) {
        return resultado; // no hay camino, arreglo vacio
    }

    // Reconstruir el camino desde destino hacia origen usando la
    // lista de predecesores, y luego invertirlo.
    ArregloDinamico<std::string> caminoInvertido;
    std::string actual = destino;
    caminoInvertido.agregarFinal(actual);

    while (actual != origen) {
        std::string encontrado_pred;
        for (int i = 0; i < predecesorDe.getTamanio(); i++) {
            if (predecesorDe[i] == actual) {
                encontrado_pred = predecesorEs[i];
                break;
            }
        }
        actual = encontrado_pred;
        caminoInvertido.agregarFinal(actual);
    }

    for (int i = caminoInvertido.getTamanio() - 1; i >= 0; i--) {
        resultado.agregarFinal(caminoInvertido[i]);
    }

    return resultado;
}

ArregloDinamico<ParSedes> GrafoSedes::listarAristasOrdenadas() const {
    ArregloDinamico<ParSedes> resultado;

    // Cada arista no dirigida aparece en ambos extremos; para no
    // listarla dos veces, solo se agrega cuando codigoA < codigoVecina
    // (orden lexicografico), asi cada par se reporta una unica vez.
    for (int i = 0; i < vertices.getTamanio(); i++) {
        const std::string& codigoA = vertices[i]->dato.codigo;
        for (int j = 0; j < vertices[i]->aristas.getTamanio(); j++) {
            const std::string& codigoB = vertices[i]->aristas[j].sedeVecina;
            if (codigoA < codigoB) {
                ParSedes par;
                par.sedeA = codigoA;
                par.sedeB = codigoB;
                par.peso = vertices[i]->aristas[j].peso;
                resultado.agregarFinal(par);
            }
        }
    }

    // Orden descendente por peso (insertion sort manual: la cantidad
    // de aristas es pequena, no hace falta nada mas sofisticado).
    for (int i = 1; i < resultado.getTamanio(); i++) {
        ParSedes actual = resultado[i];
        int j = i - 1;
        while (j >= 0 && resultado[j].peso < actual.peso) {
            resultado[j + 1] = resultado[j];
            j--;
        }
        resultado[j + 1] = actual;
    }

    return resultado;
}

ArregloDinamico<Arista> GrafoSedes::sedesSimilares(const std::string& codigo) const {
    ArregloDinamico<Arista> resultado;

    NodoGrafoSede* nodo = buscarNodo(codigo);
    if (nodo == nullptr) return resultado;

    for (int i = 0; i < nodo->aristas.getTamanio(); i++) {
        resultado.agregarFinal(nodo->aristas[i]);
    }

    // Orden descendente por peso.
    for (int i = 1; i < resultado.getTamanio(); i++) {
        Arista actual = resultado[i];
        int j = i - 1;
        while (j >= 0 && resultado[j].peso < actual.peso) {
            resultado[j + 1] = resultado[j];
            j--;
        }
        resultado[j + 1] = actual;
    }

    return resultado;
}

ArregloDinamico<Sede> GrafoSedes::listarSedes() const {
    ArregloDinamico<Sede> resultado;
    for (int i = 0; i < vertices.getTamanio(); i++) {
        resultado.agregarFinal(vertices[i]->dato);
    }
    return resultado;
}

int GrafoSedes::getCantidadVertices() const {
    return vertices.getTamanio();
}

NodoGrafoSede* GrafoSedes::getVertice(int indice) const {
    if (indice < 0 || indice >= vertices.getTamanio()) return nullptr;
    return vertices[indice];
}
