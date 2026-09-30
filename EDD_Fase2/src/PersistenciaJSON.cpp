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

// ---------------------------------------------------------------------
// Extraccion "segura" de campos JSON: si el archivo trae un campo con
// un TIPO incorrecto (ej. duracion como texto "110 minutos" en vez de
// numero), nlohmann::json lanza una excepcion al pedir j.value<int>().
// Sin este helper, esa excepcion se propagaria sin capturar y
// tumbaria toda la aplicacion en vez de solo rechazar ese registro
// (justo el escenario que el enunciado pide poder manejar: "archivos
// JSON modificados intencionalmente con... tipos de datos
// incorrectos"). Estas funciones devuelven un valor "invalido" en vez
// de lanzar, para que el resto de la validacion normal (campo vacio,
// numero negativo, etc.) lo detecte y rechace el registro con un
// mensaje claro.
// ---------------------------------------------------------------------

static std::string obtenerTexto(const json& j, const std::string& clave, std::string& errorOut) {
    if (!j.contains(clave)) return "";
    try {
        return j.at(clave).get<std::string>();
    } catch (const json::exception&) {
        errorOut = clave + " tiene un tipo de dato invalido (se esperaba texto)";
        return "";
    }
}

static int obtenerEntero(const json& j, const std::string& clave, std::string& errorOut) {
    if (!j.contains(clave)) return -1;
    try {
        return j.at(clave).get<int>();
    } catch (const json::exception&) {
        errorOut = clave + " tiene un tipo de dato invalido (se esperaba un numero entero)";
        return -1;
    }
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

    nlohmann::json listaPeliculas;
    try {
        listaPeliculas = j.value("peliculas", json::array());
    } catch (const json::exception&) {
        // "peliculas" existe pero no es un arreglo: no hay nada que cargar.
        listaPeliculas = json::array();
    }

    for (const auto& jPelicula : listaPeliculas) {
        ArregloDinamico<std::string> problemas;

        std::string errorTipo;
        std::string codigo = obtenerTexto(jPelicula, "codigo", errorTipo);
        if (!errorTipo.empty()) { problemas.agregarFinal(errorTipo); errorTipo.clear(); }

        std::string titulo = obtenerTexto(jPelicula, "titulo", errorTipo);
        if (!errorTipo.empty()) { problemas.agregarFinal(errorTipo); errorTipo.clear(); }

        std::string genero = obtenerTexto(jPelicula, "genero", errorTipo);
        if (!errorTipo.empty()) { problemas.agregarFinal(errorTipo); errorTipo.clear(); }

        std::string clasificacion = obtenerTexto(jPelicula, "clasificacion", errorTipo);
        if (!errorTipo.empty()) { problemas.agregarFinal(errorTipo); errorTipo.clear(); }

        std::string idioma = obtenerTexto(jPelicula, "idioma", errorTipo);
        if (!errorTipo.empty()) { problemas.agregarFinal(errorTipo); errorTipo.clear(); }

        int duracion = obtenerEntero(jPelicula, "duracion", errorTipo);
        if (!errorTipo.empty()) { problemas.agregarFinal(errorTipo); errorTipo.clear(); }

        std::string fechaEstreno = obtenerTexto(jPelicula, "fecha_estreno", errorTipo);
        if (!errorTipo.empty()) { problemas.agregarFinal(errorTipo); errorTipo.clear(); }

        std::string fechaFin = obtenerTexto(jPelicula, "fecha_fin", errorTipo);
        if (!errorTipo.empty()) { problemas.agregarFinal(errorTipo); errorTipo.clear(); }

        if (!campoTextoValido(codigo)) problemas.agregarFinal("codigo vacio o ausente");
        if (!codigo.empty() && arbolPeliculas.buscar(codigo) != nullptr) problemas.agregarFinal("el codigo ya existe");
        if (!campoTextoValido(titulo)) problemas.agregarFinal("titulo vacio o ausente");
        if (!campoTextoValido(genero)) problemas.agregarFinal("genero vacio o ausente");
        if (!campoTextoValido(clasificacion)) problemas.agregarFinal("clasificacion vacia o ausente");
        if (!campoTextoValido(idioma)) problemas.agregarFinal("idioma vacio o ausente");
        if (duracion <= 0) problemas.agregarFinal("duracion invalida (debe ser un entero positivo)");
        if (!esFechaValida(fechaEstreno)) problemas.agregarFinal("fecha_estreno invalida (formato esperado YYYY-MM-DD)");
        if (!esFechaValida(fechaFin)) problemas.agregarFinal("fecha_fin invalida (formato esperado YYYY-MM-DD)");

        if (!problemas.estaVacio()) {
            resultado.rechazados++;
            std::string idRef = codigo.empty() ? "(sin codigo)" : codigo;
            resultado.errores.agregarFinal("Pelicula " + idRef + " rechazada: " + unirMensajes(problemas));

            int cantidadFuncionesHuerfanas = 0;
            try {
                cantidadFuncionesHuerfanas = static_cast<int>(jPelicula.value("funciones", json::array()).size());
            } catch (const json::exception&) {
                cantidadFuncionesHuerfanas = 0;
            }
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

        nlohmann::json listaFunciones;
        try {
            listaFunciones = jPelicula.value("funciones", json::array());
        } catch (const json::exception&) {
            listaFunciones = json::array();
        }

        for (const auto& jFuncion : listaFunciones) {
            ArregloDinamico<std::string> problemasFuncion;

            std::string codigoFuncion = obtenerTexto(jFuncion, "codigo_funcion", errorTipo);
            if (!errorTipo.empty()) { problemasFuncion.agregarFinal(errorTipo); errorTipo.clear(); }

            std::string horario = obtenerTexto(jFuncion, "horario", errorTipo);
            if (!errorTipo.empty()) { problemasFuncion.agregarFinal(errorTipo); errorTipo.clear(); }

            std::string sala = obtenerTexto(jFuncion, "sala", errorTipo);
            if (!errorTipo.empty()) { problemasFuncion.agregarFinal(errorTipo); errorTipo.clear(); }

            int filas = obtenerEntero(jFuncion, "filas", errorTipo);
            if (!errorTipo.empty()) { problemasFuncion.agregarFinal(errorTipo); errorTipo.clear(); }

            int columnas = obtenerEntero(jFuncion, "columnas", errorTipo);
            if (!errorTipo.empty()) { problemasFuncion.agregarFinal(errorTipo); errorTipo.clear(); }

            std::string archivoAsientos = obtenerTexto(jFuncion, "archivo_asientos", errorTipo);
            if (!errorTipo.empty()) { problemasFuncion.agregarFinal(errorTipo); errorTipo.clear(); }

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

    nlohmann::json listaClientes;
    try {
        listaClientes = j.value("clientes", json::array());
    } catch (const json::exception&) {
        listaClientes = json::array();
    }

    for (const auto& jCliente : listaClientes) {
        ArregloDinamico<std::string> problemas;
        std::string errorTipo;

        std::string id = obtenerTexto(jCliente, "id", errorTipo);
        if (!errorTipo.empty()) { problemas.agregarFinal(errorTipo); errorTipo.clear(); }

        std::string nombre = obtenerTexto(jCliente, "nombre", errorTipo);
        if (!errorTipo.empty()) { problemas.agregarFinal(errorTipo); errorTipo.clear(); }

        std::string correo = obtenerTexto(jCliente, "correo", errorTipo);
        if (!errorTipo.empty()) { problemas.agregarFinal(errorTipo); errorTipo.clear(); }

        std::string telefono = obtenerTexto(jCliente, "telefono", errorTipo);
        if (!errorTipo.empty()) { problemas.agregarFinal(errorTipo); errorTipo.clear(); }

        std::string password = obtenerTexto(jCliente, "password", errorTipo);
        if (!errorTipo.empty()) { problemas.agregarFinal(errorTipo); errorTipo.clear(); }

        std::string tipo = "cliente";
        try {
            tipo = jCliente.value("tipo", std::string("cliente"));
        } catch (const json::exception&) {
            tipo = "cliente";
        }

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

            int cantidadReservasHuerfanas = 0;
            try {
                cantidadReservasHuerfanas = static_cast<int>(jCliente.value("reservas", json::array()).size());
            } catch (const json::exception&) {
                cantidadReservasHuerfanas = 0;
            }
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

        nlohmann::json listaReservas;
        try {
            listaReservas = jCliente.value("reservas", json::array());
        } catch (const json::exception&) {
            listaReservas = json::array();
        }

        for (const auto& jReserva : listaReservas) {
            ArregloDinamico<std::string> problemasReserva;

            std::string codigoReserva = obtenerTexto(jReserva, "codigo_reserva", errorTipo);
            if (!errorTipo.empty()) { problemasReserva.agregarFinal(errorTipo); errorTipo.clear(); }

            std::string codigoFuncion = obtenerTexto(jReserva, "codigo_funcion", errorTipo);
            if (!errorTipo.empty()) { problemasReserva.agregarFinal(errorTipo); errorTipo.clear(); }

            int fila = obtenerEntero(jReserva, "fila", errorTipo);
            if (!errorTipo.empty()) { problemasReserva.agregarFinal(errorTipo); errorTipo.clear(); }

            int columna = obtenerEntero(jReserva, "columna", errorTipo);
            if (!errorTipo.empty()) { problemasReserva.agregarFinal(errorTipo); errorTipo.clear(); }

            std::string fechaReserva = obtenerTexto(jReserva, "fecha_reserva", errorTipo);
            if (!errorTipo.empty()) { problemasReserva.agregarFinal(errorTipo); errorTipo.clear(); }

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
