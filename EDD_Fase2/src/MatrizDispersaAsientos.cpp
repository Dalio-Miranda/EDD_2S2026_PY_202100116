#include "MatrizDispersaAsientos.h"
#include <iostream>
#include <iomanip>

MatrizDispersaAsientos::MatrizDispersaAsientos()
    : numFilas(0), numColumnas(0),
      cabecerasFilas(nullptr), cabecerasColumnas(nullptr),
      totalReservados(0), funcionCreada(false) {}

MatrizDispersaAsientos::~MatrizDispersaAsientos() {
    liberarTodo();
}

void MatrizDispersaAsientos::liberarTodo() {
    if (cabecerasFilas != nullptr) {
        // Basta con recorrer las filas para liberar todos los nodos,
        // ya que cada nodo reservado aparece en exactamente una fila.
        for (int i = 0; i < numFilas; i++) {
            NodoAsiento* actual = cabecerasFilas[i];
            while (actual != nullptr) {
                NodoAsiento* siguiente = actual->siguienteEnFila;
                delete actual;
                actual = siguiente;
            }
        }
        delete[] cabecerasFilas;
        cabecerasFilas = nullptr;
    }

    if (cabecerasColumnas != nullptr) {
        delete[] cabecerasColumnas;
        cabecerasColumnas = nullptr;
    }

    numFilas = 0;
    numColumnas = 0;
    totalReservados = 0;
}

void MatrizDispersaAsientos::crearFuncion(const std::string& nuevaPelicula,
                                           const std::string& nuevoHorario,
                                           const std::string& nuevaSala,
                                           int filas, int columnas) {
    // Fase 1: solo existe una funcion a la vez. Si ya habia una,
    // la sobreescribimos liberando toda su memoria primero.
    liberarTodo();

    pelicula = nuevaPelicula;
    horario = nuevoHorario;
    sala = nuevaSala;
    numFilas = filas;
    numColumnas = columnas;

    cabecerasFilas = new NodoAsiento*[numFilas];
    for (int i = 0; i < numFilas; i++) {
        cabecerasFilas[i] = nullptr;
    }

    cabecerasColumnas = new NodoAsiento*[numColumnas];
    for (int j = 0; j < numColumnas; j++) {
        cabecerasColumnas[j] = nullptr;
    }

    totalReservados = 0;
    funcionCreada = true;
}

void MatrizDispersaAsientos::eliminarFuncion() {
    // A diferencia de crearFuncion(), aqui no se reemplaza por una
    // funcion nueva: el sistema queda sin ninguna funcion activa.
    liberarTodo();
    pelicula.clear();
    horario.clear();
    sala.clear();
    funcionCreada = false;
}

NodoAsiento* MatrizDispersaAsientos::buscarEnFila(int fila, int columna) const {
    if (fila < 1 || fila > numFilas) return nullptr;

    NodoAsiento* actual = cabecerasFilas[fila - 1];
    // La lista de la fila esta ordenada ascendentemente por columna,
    // asi que podemos cortar en cuanto nos pasamos del valor buscado.
    while (actual != nullptr && actual->columna < columna) {
        actual = actual->siguienteEnFila;
    }

    if (actual != nullptr && actual->columna == columna) {
        return actual;
    }
    return nullptr;
}

bool MatrizDispersaAsientos::reservarAsiento(int fila, int columna, const std::string& nombreCliente) {
    if (!funcionCreada) return false;
    if (fila < 1 || fila > numFilas || columna < 1 || columna > numColumnas) return false;

    if (buscarEnFila(fila, columna) != nullptr) {
        return false; // ya esta ocupado
    }

    NodoAsiento* nuevo = new NodoAsiento(fila, columna, nombreCliente);

    // Insertar ordenado en la lista de la fila (por columna).
    NodoAsiento** filaActual = &cabecerasFilas[fila - 1];
    while (*filaActual != nullptr && (*filaActual)->columna < columna) {
        filaActual = &((*filaActual)->siguienteEnFila);
    }
    nuevo->siguienteEnFila = *filaActual;
    *filaActual = nuevo;

    // Insertar ordenado en la lista de la columna (por fila).
    NodoAsiento** columnaActual = &cabecerasColumnas[columna - 1];
    while (*columnaActual != nullptr && (*columnaActual)->fila < fila) {
        columnaActual = &((*columnaActual)->siguienteEnColumna);
    }
    nuevo->siguienteEnColumna = *columnaActual;
    *columnaActual = nuevo;

    totalReservados++;
    return true;
}

bool MatrizDispersaAsientos::cancelarAsiento(int fila, int columna) {
    if (!funcionCreada) return false;
    if (fila < 1 || fila > numFilas || columna < 1 || columna > numColumnas) return false;

    // Desconectar de la lista de la fila.
    NodoAsiento** filaActual = &cabecerasFilas[fila - 1];
    while (*filaActual != nullptr && (*filaActual)->columna != columna) {
        filaActual = &((*filaActual)->siguienteEnFila);
    }
    if (*filaActual == nullptr) {
        return false; // no habia reserva en ese asiento
    }
    NodoAsiento* nodoAEliminar = *filaActual;
    *filaActual = nodoAEliminar->siguienteEnFila;

    // Desconectar de la lista de la columna.
    NodoAsiento** columnaActual = &cabecerasColumnas[columna - 1];
    while (*columnaActual != nullptr && *columnaActual != nodoAEliminar) {
        columnaActual = &((*columnaActual)->siguienteEnColumna);
    }
    if (*columnaActual != nullptr) {
        *columnaActual = nodoAEliminar->siguienteEnColumna;
    }

    delete nodoAEliminar;
    totalReservados--;
    return true;
}

bool MatrizDispersaAsientos::estaOcupado(int fila, int columna) const {
    return buscarEnFila(fila, columna) != nullptr;
}

std::string MatrizDispersaAsientos::getClienteEnAsiento(int fila, int columna) const {
    NodoAsiento* nodo = buscarEnFila(fila, columna);
    if (nodo == nullptr) return "";
    return nodo->nombreCliente;
}

std::vector<std::pair<int, int>> MatrizDispersaAsientos::buscarReservasDeCliente(const std::string& nombre) const {
    std::vector<std::pair<int, int>> resultado;

    for (int i = 0; i < numFilas; i++) {
        NodoAsiento* actual = cabecerasFilas[i];
        while (actual != nullptr) {
            if (actual->nombreCliente == nombre) {
                resultado.push_back({actual->fila, actual->columna});
            }
            actual = actual->siguienteEnFila;
        }
    }
    return resultado;
}

void MatrizDispersaAsientos::imprimirMapa() const {
    if (!funcionCreada) {
        std::cout << "No hay ninguna funcion creada todavia." << std::endl;
        return;
    }

    std::cout << "===== Funcion: " << pelicula << " - " << horario << " - " << sala << " =====" << std::endl;
    std::cout << "Asientos: " << totalReservados << " reservados, "
               << getTotalLibres() << " libres, "
               << getTotalAsientos() << " total\n" << std::endl;

    std::cout << "     ";
    for (int c = 1; c <= numColumnas; c++) {
        std::cout << std::setw(4) << c;
    }
    std::cout << std::endl;

    for (int f = 1; f <= numFilas; f++) {
        std::cout << std::setw(3) << f << " |";
        for (int c = 1; c <= numColumnas; c++) {
            if (estaOcupado(f, c)) {
                std::cout << std::setw(4) << "X";
            } else {
                std::cout << std::setw(4) << ".";
            }
        }
        std::cout << std::endl;
    }
}

bool MatrizDispersaAsientos::existeFuncion() const {
    return funcionCreada;
}

int MatrizDispersaAsientos::getNumFilas() const {
    return numFilas;
}

int MatrizDispersaAsientos::getNumColumnas() const {
    return numColumnas;
}

int MatrizDispersaAsientos::getTotalAsientos() const {
    return numFilas * numColumnas;
}

int MatrizDispersaAsientos::getTotalReservados() const {
    return totalReservados;
}

int MatrizDispersaAsientos::getTotalLibres() const {
    return getTotalAsientos() - totalReservados;
}

std::string MatrizDispersaAsientos::getPelicula() const {
    return pelicula;
}

std::string MatrizDispersaAsientos::getHorario() const {
    return horario;
}

std::string MatrizDispersaAsientos::getSala() const {
    return sala;
}

NodoAsiento* MatrizDispersaAsientos::getCabeceraFila(int fila) const {
    if (fila < 1 || fila > numFilas) return nullptr;
    return cabecerasFilas[fila - 1];
}

NodoAsiento* MatrizDispersaAsientos::getCabeceraColumna(int columna) const {
    if (columna < 1 || columna > numColumnas) return nullptr;
    return cabecerasColumnas[columna - 1];
}