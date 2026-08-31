#ifndef ARBOL_AVL_FUNCIONES_H
#define ARBOL_AVL_FUNCIONES_H

#include <string>
#include <vector>
#include "Funcion.h"

// Nodo del Arbol AVL. La clave de orden es codigoFuncion (comparacion
// lexicografica de string, ej. "F001" < "F002" < "F010").
struct NodoAVL {
    Funcion dato;
    NodoAVL* izquierdo;
    NodoAVL* derecho;
    int altura; // altura del subarbol con raiz en este nodo (hoja = 1)

    explicit NodoAVL(const Funcion& f)
        : dato(f), izquierdo(nullptr), derecho(nullptr), altura(1) {}
};

// Recorrido a usar en listarFunciones().
enum class TipoRecorrido { PREORDEN, INORDEN, POSTORDEN };

// Arbol AVL que administra las funciones de cine, indexado por
// codigo_funcion. Implementacion 100% manual (sin usar std::map,
// std::set ni ninguna estructura de arbol de la STL), tal como exige
// el enunciado de Fase 2 (seccion 4.2, Alcance del proyecto).
//
// El balance automatico garantiza que buscar/insertar/eliminar una
// funcion siga siendo O(log n) sin importar cuantas funciones se
// creen o eliminen -- esto es justamente lo que resuelve la
// limitacion de Fase 1 de solo poder manejar una funcion a la vez.
class ArbolAVLFunciones {
private:
    NodoAVL* raiz;
    int tamanio;

    // ---- helpers internos de balanceo ----
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

    void preordenRec(NodoAVL* nodo, std::vector<Funcion>& salida) const;
    void inordenRec(NodoAVL* nodo, std::vector<Funcion>& salida) const;
    void postordenRec(NodoAVL* nodo, std::vector<Funcion>& salida) const;

public:
    ArbolAVLFunciones();
    ~ArbolAVLFunciones();

    // Los objetos ArbolAVLFunciones no se copian: la Fase 2 los usa
    // siempre por referencia compartida entre paneles (mismo patron
    // que ListaCircularSolicitudes en Fase 1), asi que deshabilitamos
    // copia para evitar duplicar el arbol por error.
    ArbolAVLFunciones(const ArbolAVLFunciones&) = delete;
    ArbolAVLFunciones& operator=(const ArbolAVLFunciones&) = delete;

    // Inserta una funcion nueva. Retorna false si el codigo_funcion
    // ya existe (no se sobreescribe; para eso esta editarFuncion).
    bool insertar(const Funcion& f);

    // Elimina el nodo del AVL manteniendo el balance. NO borra el
    // archivo F00X_funcion.json en disco -- eso es responsabilidad de
    // quien llama (el panel de administrador), junto con la logica de
    // "si la funcion eliminada era la activa en la matriz dispersa,
    // primero hay que descargarla". Mantener esa orquestacion fuera
    // del arbol evita acoplar la estructura de datos con la UI/IO.
    bool eliminar(const std::string& codigoFuncion);

    // Edita los campos de una funcion existente sin tocar su posicion
    // en el arbol (el codigo_funcion, que es la clave, no cambia).
    bool editar(const std::string& codigoFuncion, const Funcion& datosNuevos);

    Funcion* buscar(const std::string& codigoFuncion);
    bool existe(const std::string& codigoFuncion) const;

    int getTamanio() const;
    bool estaVacio() const;

    // Recorridos para "listar todas las funciones existentes" (punto
    // 03 del enunciado). Solo lee los nodos del AVL, nunca abre los
    // archivos de asientos de cada funcion.
    std::vector<Funcion> listarFunciones(TipoRecorrido tipo) const;

    NodoAVL* getRaiz() const; // usado por ReportesGraphviz para dibujar el arbol
};

#endif // ARBOL_AVL_FUNCIONES_H
