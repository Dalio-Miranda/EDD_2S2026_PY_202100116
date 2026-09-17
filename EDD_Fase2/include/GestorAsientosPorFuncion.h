#ifndef GESTOR_ASIENTOS_POR_FUNCION_H
#define GESTOR_ASIENTOS_POR_FUNCION_H

#include <string>
#include "MatrizDispersaAsientos.h"
#include "ArbolAVLFunciones.h"

// Orquesta el ciclo de vida de la Matriz Dispersa en Fase 2: la
// matriz se carga y vacia segun la funcion seleccionada, persistiendo
// cada funcion en su propio archivo F00X_funcion.json (enunciado 4.1,
// puntos 01 y 03).
//
// NO modifica MatrizDispersaAsientos (la implementacion interna de
// Fase 1 se reutiliza tal cual). El campo "nombreCliente" de cada
// NodoAsiento se reutiliza aqui para guardar el codigo_reserva.
class GestorAsientosPorFuncion {
private:
    MatrizDispersaAsientos matrizActiva;
    std::string codigoFuncionActiva;
    std::string carpetaAsientos;

    std::string rutaArchivo(const std::string& codigoFuncion) const;
    void escribirArchivoActiva() const;
    void cargarDesdeArchivo(const std::string& codigoFuncion);

public:
    explicit GestorAsientosPorFuncion(std::string carpetaAsientos = "data/asientos");

    GestorAsientosPorFuncion(const GestorAsientosPorFuncion&) = delete;
    GestorAsientosPorFuncion& operator=(const GestorAsientosPorFuncion&) = delete;

    bool cargarFuncion(const std::string& codigoFuncion, ArbolAVLFunciones& arbolFunciones);
    void persistirActiva() const;
    void descargarActiva();

    bool hayFuncionActiva() const;
    std::string getCodigoFuncionActiva() const;

    MatrizDispersaAsientos& getMatrizActiva();

    bool reservarAsiento(int fila, int columna, const std::string& codigoReserva);
    bool cancelarAsiento(int fila, int columna);

    bool eliminarArchivoFuncion(const std::string& codigoFuncion);
};

#endif // GESTOR_ASIENTOS_POR_FUNCION_H
