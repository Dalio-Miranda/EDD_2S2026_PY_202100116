#include "PersistenciaJSON.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <cctype>
#include <ctime>

using json = nlohmann::json;

bool PersistenciaJSON::campoTextoValido(const std::string& valor) {
    return !valor.empty();
}

std::string PersistenciaJSON::fechaHoy() {
    std::time_t ahora = std::time(nullptr);
    std::tm* t = std::localtime(&ahora);
    char buffer[16];
    std::snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d",
                  t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    return std::string(buffer);
}

bool PersistenciaJSON::esFechaValida(const std::string& fecha) {
    if (fecha.size() != 10) return false;
    if (fecha[4] != '-' || fecha[7] != '-') return false;

    for (int i : {0, 1, 2, 3, 5, 6, 8, 9}) {
        if (!std::isdigit(static_cast<unsigned char>(fecha[static_cast<size_t>(i)]))) {
            return false;
        }
    }

    int anio = std::stoi(fecha.substr(0, 4));
    int mes = std::stoi(fecha.substr(5, 2));
    int dia = std::stoi(fecha.substr(8, 2));

    if (anio < 1900 || anio > 2100) return false;
    if (mes < 1 || mes > 12) return false;

    static const int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int maxDia = diasPorMes[mes - 1];
    if (mes == 2) {
        bool bisiesto = (anio % 4 == 0 && anio % 100 != 0) || (anio % 400 == 0);
        if (bisiesto) maxDia = 29;
    }

    return dia >= 1 && dia <= maxDia;
}

// Concatena los mensajes de un ArregloDinamico<string> separados por
// "; ", replicando lo que antes hacia un join manual sobre
// std::vector.
static std::string unirMensajes(const ArregloDinamico<std::string>& problemas) {
    std::string msg;
    for (int i = 0; i < problemas.getTamanio(); i++) {
        msg += problemas[i];
        if (i + 1 < problemas.getTamanio()) msg += "; ";
    }
    return msg;
}

ResultadoCarga PersistenciaJSON::cargarPeliculasYFunciones(
    const std::string& rutaArchivo,
    ArbolBinarioPeliculas& arbolPeliculas,
    ArbolAVLFunciones& arbolFunciones) {

    ResultadoCarga resultado;

    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        resultado.huboErrorFatal = true;
        resultado.errores.agregarFinal("No se pudo abrir el archivo: " + rutaArchivo);
        return resultado;
    }

    json j;
    try {
        archivo >> j;
    } catch (const json::parse_error& e) {
        resultado.huboErrorFatal = true;
        resultado.errores.agregarFinal(std::string("El archivo no es un JSON valido: ") + e.what());
        return resultado;
    }

    for (const auto& jPelicula : j.value("peliculas", json::array())) {
        std::string codigo = jPelicula.value("codigo", "");
        std::string titulo = jPelicula.value("titulo", "");

        ArregloDinamico<std::string> problemas;
        if (!campoTextoValido(codigo)) problemas.agregarFinal("codigo vacio o ausente");
        if (!codigo.empty() && arbolPeliculas.buscar(codigo) != nullptr) problemas.agregarFinal("el codigo ya existe");
        if (!campoTextoValido(titulo)) problemas.agregarFinal("titulo vacio o ausente");

        std::string genero = jPelicula.value("genero", "");
        std::string clasificacion = jPelicula.value("clasificacion", "");
        std::string idioma = jPelicula.value("idioma", "");
        if (!campoTextoValido(genero)) problemas.agregarFinal("genero vacio o ausente");
        if (!campoTextoValido(clasificacion)) problemas.agregarFinal("clasificacion vacia o ausente");
        if (!campoTextoValido(idioma)) problemas.agregarFinal("idioma vacio o ausente");

        int duracion = jPelicula.value("duracion", -1);
        if (duracion <= 0) problemas.agregarFinal("duracion invalida (debe ser un entero positivo)");

        std::string fechaEstreno = jPelicula.value("fecha_estreno", "");
        std::string fechaFin = jPelicula.value("fecha_fin", "");
        if (!esFechaValida(fechaEstreno)) problemas.agregarFinal("fecha_estreno invalida (formato esperado YYYY-MM-DD)");
        if (!esFechaValida(fechaFin)) problemas.agregarFinal("fecha_fin invalida (formato esperado YYYY-MM-DD)");

        if (!problemas.estaVacio()) {
            resultado.rechazados++;
            std::string idRef = codigo.empty() ? "(sin codigo)" : codigo;
            resultado.errores.agregarFinal("Pelicula " + idRef + " rechazada: " + unirMensajes(problemas));

            int cantidadFuncionesHuerfanas = static_cast<int>(jPelicula.value("funciones", json::array()).size());
            if (cantidadFuncionesHuerfanas > 0) {
                resultado.rechazados += cantidadFuncionesHuerfanas;
                resultado.errores.agregarFinal(
                    "  -> " + std::to_string(cantidadFuncionesHuerfanas) +
                    " funcion(es) de esa pelicula tambien rechazadas");
            }
            continue;
        }

        Pelicula p;
        p.codigo = codigo;
        p.titulo = titulo;
        p.genero = genero;
        p.duracion = duracion;
        p.clasificacion = clasificacion;
        p.idioma = idioma;
        p.fechaEstreno = fechaEstreno;
        p.fechaFin = fechaFin;
        arbolPeliculas.insertar(p);
        resultado.exitosos++;

        for (const auto& jFuncion : jPelicula.value("funciones", json::array())) {
            std::string codigoFuncion = jFuncion.value("codigo_funcion", "");
            std::string horario = jFuncion.value("horario", "");
            std::string sala = jFuncion.value("sala", "");
            int filas = jFuncion.value("filas", -1);
            int columnas = jFuncion.value("columnas", -1);
            std::string archivoAsientos = jFuncion.value("archivo_asientos", "");

            ArregloDinamico<std::string> problemasFuncion;
            if (!campoTextoValido(codigoFuncion)) problemasFuncion.agregarFinal("codigo_funcion vacio o ausente");
            if (!campoTextoValido(horario)) problemasFuncion.agregarFinal("horario vacio o ausente");
            if (!campoTextoValido(sala)) problemasFuncion.agregarFinal("sala vacia o ausente");
            if (filas <= 0) problemasFuncion.agregarFinal("filas invalidas (debe ser un entero positivo)");
            if (columnas <= 0) problemasFuncion.agregarFinal("columnas invalidas (debe ser un entero positivo)");

            if (!problemasFuncion.estaVacio()) {
                resultado.rechazados++;
                std::string idRef = codigoFuncion.empty() ? "(sin codigo)" : codigoFuncion;
                resultado.errores.agregarFinal("Funcion " + idRef + " de pelicula " + codigo +
                                                " rechazada: " + unirMensajes(problemasFuncion));
                continue;
            }

            Funcion f(codigoFuncion, codigo, horario, sala, filas, columnas, archivoAsientos);
            bool ok = arbolFunciones.insertar(f);
            if (ok) {
                resultado.exitosos++;
            } else {
                resultado.rechazados++;
                resultado.errores.agregarFinal("Funcion " + codigoFuncion + " rechazada: el codigo_funcion ya existe");
            }
        }
    }

    return resultado;
}

ResultadoCarga PersistenciaJSON::cargarClientesYReservas(
    const std::string& rutaArchivo,
    ArbolBClientes& arbolClientes,
    TablaHashReservas& tablaReservas) {

    ResultadoCarga resultado;

    std::ifstream archivo(rutaArchivo);
    if (!archivo.is_open()) {
        resultado.huboErrorFatal = true;
        resultado.errores.agregarFinal("No se pudo abrir el archivo: " + rutaArchivo);
        return resultado;
    }

    json j;
    try {
        archivo >> j;
    } catch (const json::parse_error& e) {
        resultado.huboErrorFatal = true;
        resultado.errores.agregarFinal(std::string("El archivo no es un JSON valido: ") + e.what());
        return resultado;
    }

    for (const auto& jCliente : j.value("clientes", json::array())) {
        std::string id = jCliente.value("id", "");
        std::string nombre = jCliente.value("nombre", "");
        std::string correo = jCliente.value("correo", "");
        std::string telefono = jCliente.value("telefono", "");
        std::string password = jCliente.value("password", "");
        std::string tipo = jCliente.value("tipo", "cliente");

        ArregloDinamico<std::string> problemas;
        if (!campoTextoValido(id)) problemas.agregarFinal("id vacio o ausente");
        if (!campoTextoValido(nombre)) problemas.agregarFinal("nombre vacio o ausente");
        if (correo.find('@') == std::string::npos || correo.empty()) {
            problemas.agregarFinal("correo invalido (debe contener '@')");
        }
        if (!campoTextoValido(telefono)) problemas.agregarFinal("telefono vacio o ausente");
        if (!campoTextoValido(password)) problemas.agregarFinal("password vacio o ausente");

        if (!problemas.estaVacio()) {
            resultado.rechazados++;
            std::string idRef = id.empty() ? "(sin id)" : id;
            resultado.errores.agregarFinal("Cliente " + idRef + " rechazado: " + unirMensajes(problemas));

            int cantidadReservasHuerfanas = static_cast<int>(jCliente.value("reservas", json::array()).size());
            if (cantidadReservasHuerfanas > 0) {
                resultado.rechazados += cantidadReservasHuerfanas;
                resultado.errores.agregarFinal(
                    "  -> " + std::to_string(cantidadReservasHuerfanas) +
                    " reserva(s) de ese cliente tambien rechazadas");
            }
            continue;
        }

        Cliente c(id, nombre, correo, telefono, password, tipo);
        bool okCliente = arbolClientes.insertar(c);
        if (!okCliente) {
            resultado.rechazados++;
            resultado.errores.agregarFinal("Cliente " + id + " rechazado: el id ya existe");
            continue;
        }
        resultado.exitosos++;

        ArregloDinamico<std::string> codigosValidos;
        for (const auto& jReserva : jCliente.value("reservas", json::array())) {
            std::string codigoReserva = jReserva.value("codigo_reserva", "");
            std::string codigoFuncion = jReserva.value("codigo_funcion", "");
            int fila = jReserva.value("fila", -1);
            int columna = jReserva.value("columna", -1);
            std::string fechaReserva = jReserva.value("fecha_reserva", "");

            ArregloDinamico<std::string> problemasReserva;
            if (!campoTextoValido(codigoReserva)) problemasReserva.agregarFinal("codigo_reserva vacio o ausente");
            if (!campoTextoValido(codigoFuncion)) problemasReserva.agregarFinal("codigo_funcion vacio o ausente");
            if (fila <= 0) problemasReserva.agregarFinal("fila invalida (debe ser un entero positivo)");
            if (columna <= 0) problemasReserva.agregarFinal("columna invalida (debe ser un entero positivo)");
            if (!esFechaValida(fechaReserva)) problemasReserva.agregarFinal("fecha_reserva invalida (formato esperado YYYY-MM-DD)");

            if (!problemasReserva.estaVacio()) {
                resultado.rechazados++;
                std::string idRef = codigoReserva.empty() ? "(sin codigo)" : codigoReserva;
                resultado.errores.agregarFinal("Reserva " + idRef + " de cliente " + id +
                                                " rechazada: " + unirMensajes(problemasReserva));
                continue;
            }

            Reserva r(codigoReserva, codigoFuncion, id, fila, columna, fechaReserva);
            bool okReserva = tablaReservas.insertar(r);
            if (okReserva) {
                codigosValidos.agregarFinal(codigoReserva);
                resultado.exitosos++;
            } else {
                resultado.rechazados++;
                resultado.errores.agregarFinal("Reserva " + codigoReserva + " rechazada: el codigo_reserva ya existe");
            }
        }

        Cliente* clienteInsertado = arbolClientes.buscar(id);
        if (clienteInsertado != nullptr) {
            clienteInsertado->codigosReserva = codigosValidos;
        }
    }

    return resultado;
}
