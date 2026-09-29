#ifndef FUNCION_H
#define FUNCION_H

#include <string>
#include <nlohmann/json.hpp>

struct Funcion {
    std::string codigoFuncion;
    std::string codigoPelicula;
    std::string horario;
    std::string sala;
    int filas = 0;
    int columnas = 0;
    std::string archivoAsientos;
    bool yaRealizada = false;

    // --- Nuevo en Fase 3 ---
    std::string codigoSede; // codigo de la Sede (vertice del grafo) donde se proyecta
    std::string fecha;      // formato YYYY-MM-DD (antes solo existia "horario", hora del dia)

    Funcion() = default;

    Funcion(std::string codigoFuncion,
            std::string codigoPelicula,
            std::string horario,
            std::string sala,
            int filas,
            int columnas,
            std::string archivoAsientos = "",
            std::string codigoSede = "",
            std::string fecha = "")
        : codigoFuncion(std::move(codigoFuncion)),
          codigoPelicula(std::move(codigoPelicula)),
          horario(std::move(horario)),
          sala(std::move(sala)),
          filas(filas),
          columnas(columnas),
          archivoAsientos(std::move(archivoAsientos)),
          codigoSede(std::move(codigoSede)),
          fecha(std::move(fecha)) {}

    static std::string nombreArchivoPara(const std::string& codigoFuncion) {
        return codigoFuncion + "_funcion.json";
    }
};

void to_json(nlohmann::json& j, const Funcion& f);
void from_json(const nlohmann::json& j, Funcion& f);

#endif // FUNCION_H
