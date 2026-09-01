#include "GestorAsientosPorFuncion.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

GestorAsientosPorFuncion::GestorAsientosPorFuncion(std::string carpetaAsientos)
    : codigoFuncionActiva(""), carpetaAsientos(std::move(carpetaAsientos)) {
    // Crea la carpeta si no existe, para que escribirArchivoActiva()
    // nunca falle por una carpeta faltante.
    fs::create_directories(this->carpetaAsientos);
}

std::string GestorAsientosPorFuncion::rutaArchivo(const std::string& codigoFuncion) const {
    fs::path base(carpetaAsientos);
    return (base / Funcion::nombreArchivoPara(codigoFuncion)).string();
}

void GestorAsientosPorFuncion::escribirArchivoActiva() const {
    if (codigoFuncionActiva.empty()) return;

    nlohmann::json asientosOcupados = nlohmann::json::array();

    // Recorre fila por fila la matriz activa, siguiendo la lista
    // enlazada de cada cabecera de fila (solo visita asientos
    // OCUPADOS, que es justamente la propiedad de una matriz dispersa).
    for (int f = 0; f < matrizActiva.getNumFilas(); f++) {
        for (NodoAsiento* nodo = matrizActiva.getCabeceraFila(f); nodo != nullptr; nodo = nodo->siguienteEnFila) {
            // nodo->nombreCliente guarda aqui el codigo_reserva (ver
            // nota de diseno en el .h): es el mismo campo string
            // generico de MatrizDispersaAsientos, reutilizado con otro
            // significado en el flujo de Fase 2.
            asientosOcupados.push_back({
                {"fila", nodo->fila},
                {"columna", nodo->columna},
                {"codigo_reserva", nodo->nombreCliente}
            });
        }
    }

    nlohmann::json j = {
        {"codigo_funcion", codigoFuncionActiva},
        {"asientos_ocupados", asientosOcupados}
    };

    std::ofstream archivo(rutaArchivo(codigoFuncionActiva));
    archivo << j.dump(2);
}

void GestorAsientosPorFuncion::cargarDesdeArchivo(const std::string& codigoFuncion) {
    std::string ruta = rutaArchivo(codigoFuncion);
    if (!fs::exists(ruta)) {
        // Funcion recien creada / sin reservas todavia: no es un
        // error, la matriz simplemente queda vacia.
        return;
    }

    std::ifstream archivo(ruta);
    nlohmann::json j;
    archivo >> j;

    for (const auto& entrada : j.value("asientos_ocupados", nlohmann::json::array())) {
        int fila = entrada.at("fila").get<int>();
        int columna = entrada.at("columna").get<int>();
        std::string codigoReserva = entrada.at("codigo_reserva").get<std::string>();

        // Reutiliza reservarAsiento() de Fase 1 tal cual, pasando el
        // codigo_reserva en el parametro que originalmente representa
        // el nombre del cliente (ver nota de diseno en el .h).
        matrizActiva.reservarAsiento(fila, columna, codigoReserva);
    }
}

bool GestorAsientosPorFuncion::cargarFuncion(const std::string& codigoFuncion, ArbolAVLFunciones& arbolFunciones) {
    Funcion* funcion = arbolFunciones.buscar(codigoFuncion);
    if (funcion == nullptr) return false;

    // Si ya habia otra funcion activa (distinta de la que se pide
    // cargar), se persiste y se descarga antes de continuar.
    if (!codigoFuncionActiva.empty() && codigoFuncionActiva != codigoFuncion) {
        persistirActiva();
        matrizActiva.eliminarFuncion();
        codigoFuncionActiva.clear();
    }

    // Si ya es la funcion activa, no hace falta recargar desde cero.
    if (codigoFuncionActiva == codigoFuncion) {
        return true;
    }

    matrizActiva.crearFuncion(funcion->codigoPelicula, funcion->horario, funcion->sala,
                               funcion->filas, funcion->columnas);
    codigoFuncionActiva = codigoFuncion;

    cargarDesdeArchivo(codigoFuncion);

    return true;
}

void GestorAsientosPorFuncion::persistirActiva() const {
    escribirArchivoActiva();
}

void GestorAsientosPorFuncion::descargarActiva() {
    if (codigoFuncionActiva.empty()) return;
    persistirActiva();
    matrizActiva.eliminarFuncion();
    codigoFuncionActiva.clear();
}

bool GestorAsientosPorFuncion::hayFuncionActiva() const {
    return !codigoFuncionActiva.empty();
}

std::string GestorAsientosPorFuncion::getCodigoFuncionActiva() const {
    return codigoFuncionActiva;
}

MatrizDispersaAsientos& GestorAsientosPorFuncion::getMatrizActiva() {
    return matrizActiva;
}

bool GestorAsientosPorFuncion::reservarAsiento(int fila, int columna, const std::string& codigoReserva) {
    if (codigoFuncionActiva.empty()) return false;

    bool ok = matrizActiva.reservarAsiento(fila, columna, codigoReserva);
    if (ok) {
        persistirActiva();
    }
    return ok;
}

bool GestorAsientosPorFuncion::cancelarAsiento(int fila, int columna) {
    if (codigoFuncionActiva.empty()) return false;

    bool ok = matrizActiva.cancelarAsiento(fila, columna);
    if (ok) {
        persistirActiva();
    }
    return ok;
}

bool GestorAsientosPorFuncion::eliminarArchivoFuncion(const std::string& codigoFuncion) {
    if (codigoFuncionActiva == codigoFuncion) {
        // No tiene sentido persistir algo que se va a borrar de inmediato.
        matrizActiva.eliminarFuncion();
        codigoFuncionActiva.clear();
    }

    std::string ruta = rutaArchivo(codigoFuncion);
    if (!fs::exists(ruta)) return false;

    return fs::remove(ruta);
}
