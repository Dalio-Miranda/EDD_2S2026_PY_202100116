#ifndef GRAFO_SEDES_H
#define GRAFO_SEDES_H

#include <string>
#include "ArregloDinamico.h"
#include "Sede.h"
#include "ArbolAVLFunciones.h"

// Una arista (vista desde un vertice): hacia que sede vecina, y con
// que peso (cantidad de peliculas distintas que ambas sedes tienen
// en comun en su cartelera).
struct Arista {
    std::string sedeVecina;
    int peso = 0;
};

// Nodo del grafo: una sede, mas su lista de adyacencia (las aristas
// que salen de ella). Como el grafo NO es dirigido, cada arista real
// aparece en las listas de AMBOS extremos.
struct NodoGrafoSede {
    Sede dato;
    ArregloDinamico<Arista> aristas;

    explicit NodoGrafoSede(const Sede& s) : dato(s) {}
};

// Un par de sedes con el peso de su relacion, usado para el ranking
// de "sedes con mayor relacion" (enunciado 4.1, punto 2.e).
struct ParSedes {
    std::string sedeA;
    std::string sedeB;
    int peso = 0;
};

// Grafo NO dirigido ponderado que modela las sedes del cine como
// vertices y su relacion de "cartelera compartida" como aristas.
// Implementacion 100% manual con lista de adyacencia (no matriz de
// adyacencia): el enunciado justifica esta eleccion porque, con un
// numero moderado de sedes y relaciones que no se dan entre TODOS
// los pares posibles, la lista aprovecha mejor el espacio guardando
// solo las conexiones que realmente existen.
//
// Los vertices se guardan como punteros (NodoGrafoSede*) dentro de un
// ArregloDinamico, para que sus direcciones de memoria NUNCA cambien
// aunque el arreglo se redimensione (evita invalidar punteros/referencias
// que la interfaz pueda estar sosteniendo hacia una Sede en particular).
class GrafoSedes {
private:
    ArregloDinamico<NodoGrafoSede*> vertices;

    NodoGrafoSede* buscarNodo(const std::string& codigo) const;
    int indiceDeArista(NodoGrafoSede* nodo, const std::string& codigoVecino) const;

    // Establece (crea/actualiza/elimina) la arista UNIDIRECCIONAL vista
    // desde "nodo" hacia "codigoVecino". Uso interno de establecerPeso().
    void establecerAristaUnidireccional(NodoGrafoSede* nodo, const std::string& codigoVecino, int peso);

public:
    GrafoSedes() = default;
    ~GrafoSedes();

    GrafoSedes(const GrafoSedes&) = delete;
    GrafoSedes& operator=(const GrafoSedes&) = delete;

    // Inserta una sede nueva como vertice, sin aristas. Retorna false
    // si el codigo ya existe.
    bool agregarSede(const Sede& s);

    Sede* buscarSede(const std::string& codigo);

    // Edita los datos de una sede existente. El CODIGO no cambia (las
    // funciones lo referencian). Retorna false si la sede no existe.
    bool editarSede(const std::string& codigo,
                    const std::string& nombre,
                    const std::string& direccion,
                    int cantidadSalas,
                    const std::string& telefono);

    // Elimina una sede (vertice) y TODAS las aristas que la tocan: la
    // arista de cada vecino hacia ella se quita de su lista de
    // adyacencia antes de liberar el nodo. Las funciones de esa sede
    // las maneja quien llama (ver PanelAdminSedes). Retorna false si
    // no existe.
    bool eliminarSede(const std::string& codigo);
    bool existeSede(const std::string& codigo) const;
    int getCantidadSedes() const;

    // Establece el peso de la arista NO DIRIGIDA entre codigoA y
    // codigoB (actualiza ambos extremos). Si peso <= 0, la arista se
    // ELIMINA (si existia); si peso > 0, se crea o actualiza.
    void establecerPeso(const std::string& codigoA, const std::string& codigoB, int peso);

    // Peso actual de la arista entre codigoA y codigoB, o 0 si no
    // existe relacion entre ambas sedes.
    int getPeso(const std::string& codigoA, const std::string& codigoB) const;

    // Recalcula TODAS las aristas que tocan a codigoSede, comparando
    // su catalogo de peliculas (obtenido de "funciones") contra el de
    // cada una de las demas sedes del grafo. Se debe llamar despues
    // de crear, editar o eliminar cualquier funcion (enunciado 4.1,
    // "Construccion de las aristas").
    void recalcularRelacionesDeSede(const std::string& codigoSede, ArbolAVLFunciones& funciones);

    // BFS: la secuencia de sedes (codigos) con MENOR CANTIDAD DE
    // SALTOS entre origen y destino, incluyendolos a ambos. Arreglo
    // vacio si no existe camino. Se usa BFS (no Dijkstra) porque lo
    // que importa aqui es la cantidad de aristas recorridas, no la
    // suma de sus pesos (enunciado 4.1, punto 2.d).
    ArregloDinamico<std::string> caminoMasCorto(const std::string& origen, const std::string& destino) const;

    // Todas las aristas del grafo (cada par UNA sola vez, no
    // duplicada desde ambos extremos), ordenadas de mayor a menor
    // peso -- para el ranking de sedes con mayor relacion.
    ArregloDinamico<ParSedes> listarAristasOrdenadas() const;

    // Sedes vecinas de "codigo", ordenadas de mayor a menor peso --
    // para "sedes similares a la sede seleccionada" (enunciado,
    // funciones de cliente).
    ArregloDinamico<Arista> sedesSimilares(const std::string& codigo) const;

    ArregloDinamico<Sede> listarSedes() const;

    // Acceso a la estructura interna, para los reportes de Graphviz.
    int getCantidadVertices() const;
    NodoGrafoSede* getVertice(int indice) const;
};

#endif // GRAFO_SEDES_H
