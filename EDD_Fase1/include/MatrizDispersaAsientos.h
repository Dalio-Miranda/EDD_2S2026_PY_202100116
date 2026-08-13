#ifndef MATRIZ_DISPERSA_ASIENTOS_H
#define MATRIZ_DISPERSA_ASIENTOS_H

#include <string>
#include <vector>
#include <utility>

// Nodo de la matriz dispersa. Cada asiento RESERVADO es un nodo que
// vive simultaneamente en dos listas enlazadas: la de su fila y la
// de su columna. Los asientos LIBRES no generan ningun nodo, por
// eso la matriz es "dispersa": solo ocupa memoria lo que tiene datos.
struct NodoAsiento {
    int fila;
    int columna;
    std::string nombreCliente;

    NodoAsiento* siguienteEnFila;    // siguiente asiento reservado en la misma fila (columna mayor)
    NodoAsiento* siguienteEnColumna; // siguiente asiento reservado en la misma columna (fila mayor)

    NodoAsiento(int f, int c, const std::string& cliente)
        : fila(f), columna(c), nombreCliente(cliente),
          siguienteEnFila(nullptr), siguienteEnColumna(nullptr) {}
};

// Matriz dispersa que representa el mapa de asientos de UNA funcion
// de cine (pelicula + horario + sala). Segun el enunciado de Fase 1,
// solo existe una funcion a la vez: crear una nueva sobreescribe
// la anterior.
class MatrizDispersaAsientos {
private:
    int numFilas;
    int numColumnas;

    NodoAsiento** cabecerasFilas;    // arreglo de punteros, tamano numFilas
    NodoAsiento** cabecerasColumnas; // arreglo de punteros, tamano numColumnas

    int totalReservados;

    std::string pelicula;
    std::string horario;
    std::string sala;

    bool funcionCreada;

public:
    MatrizDispersaAsientos();
    ~MatrizDispersaAsientos();

    // Crea (o sobreescribe) la funcion actual con una sala vacia
    // de numFilas x numColumnas, todos los asientos LIBRES.
    void crearFuncion(const std::string& pelicula, const std::string& horario,
                       const std::string& sala, int filas, int columnas);

    // Reserva un asiento. Retorna false si esta fuera de rango o ya ocupado.
    bool reservarAsiento(int fila, int columna, const std::string& nombreCliente);

    // Libera un asiento reservado. Retorna false si no existia la reserva.
    bool cancelarAsiento(int fila, int columna);

    bool estaOcupado(int fila, int columna) const;
    std::string getClienteEnAsiento(int fila, int columna) const; // "" si esta libre

    // Retorna todas las reservas (fila, columna) de un cliente por nombre,
    // usado para la funcion de "cancelar reserva" del cliente.
    std::vector<std::pair<int, int>> buscarReservasDeCliente(const std::string& nombre) const;

    void imprimirMapa() const;

    bool existeFuncion() const;
    int getNumFilas() const;
    int getNumColumnas() const;
    int getTotalAsientos() const;
    int getTotalReservados() const;
    int getTotalLibres() const;

    std::string getPelicula() const;
    std::string getHorario() const;
    std::string getSala() const;

    // Acceso a las cabeceras, util para el reporte de Graphviz mas adelante.
    NodoAsiento* getCabeceraFila(int fila) const;
    NodoAsiento* getCabeceraColumna(int columna) const;

private:
    NodoAsiento* buscarEnFila(int fila, int columna) const;
    void liberarTodo();
};

#endif // MATRIZ_DISPERSA_ASIENTOS_H