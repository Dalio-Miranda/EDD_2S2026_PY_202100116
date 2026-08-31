#include "ReportesGraphviz.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <vector>

namespace ReportesGraphviz {

// ===================== Helpers internos =====================

// Escapa comillas dobles para que no rompan el archivo .dot.
static std::string escapar(const std::string& texto) {
    std::string resultado;
    for (char c : texto) {
        if (c == '"') resultado += "\\\"";
        else resultado += c;
    }
    return resultado;
}

// Calcula cuantos dias faltan entre hoy y una fecha "AAAA-MM-DD".
// Retorna un numero negativo si la fecha ya paso.
static int diasHastaFecha(const std::string& fecha) {
    int anio, mes, dia;
    if (std::sscanf(fecha.c_str(), "%d-%d-%d", &anio, &mes, &dia) != 3) {
        return 9999; // fecha invalida: no marcar como proxima a vencer
    }

    std::tm objetivo = {};
    objetivo.tm_year = anio - 1900;
    objetivo.tm_mon = mes - 1;
    objetivo.tm_mday = dia;
    std::time_t tiempoObjetivo = std::mktime(&objetivo);

    std::time_t ahora = std::time(nullptr);
    std::tm* hoyTm = std::localtime(&ahora);
    std::tm hoyCopia = *hoyTm;
    hoyCopia.tm_hour = 0;
    hoyCopia.tm_min = 0;
    hoyCopia.tm_sec = 0;
    std::time_t tiempoHoy = std::mktime(&hoyCopia);

    double segundos = std::difftime(tiempoObjetivo, tiempoHoy);
    return static_cast<int>(segundos / (60 * 60 * 24));
}

// Escribe el .dot a disco y ejecuta "dot" para generar el .png.
static void ejecutarDot(const std::string& contenidoDot, const std::string& rutaBase) {
    std::string rutaDot = rutaBase + ".dot";
    std::string rutaPng = rutaBase + ".png";

    std::ofstream archivo(rutaDot);
    if (!archivo.is_open()) {
        std::cerr << "No se pudo crear el archivo: " << rutaDot << std::endl;
        return;
    }
    archivo << contenidoDot;
    archivo.close();

    std::string comando = "dot -Tpng \"" + rutaDot + "\" -o \"" + rutaPng + "\"";
    int resultado = std::system(comando.c_str());

    if (resultado == 0) {
        std::cout << "Reporte generado: " << rutaPng << std::endl;
    } else {
        std::cerr << "Fallo al generar el reporte con Graphviz (revisa que 'dot' este en el PATH)." << std::endl;
    }
}

// ===================== Reporte 1: Arbol de peliculas =====================

// Recorre el arbol recursivamente y escribe cada nodo + sus conexiones.
static void escribirNodoArbol(NodoArbolPelicula* nodo, std::ostringstream& dot) {
    if (nodo == nullptr) return;

    const Pelicula& p = nodo->dato;
    int dias = diasHastaFecha(p.fechaFin);
    std::string color = (dias <= 7) ? "#FFE066" : "#8BE28B"; // amarillo : verde

    dot << "    \"" << p.codigo << "\" [label=\"Codigo: " << p.codigo
        << "\\lTitulo: " << escapar(p.titulo)
        << "\\lDuracion: " << p.duracion << " min"
        << "\\lClasificacion: " << p.clasificacion
        << "\\l\", fillcolor=\"" << color << "\"];\n";

    if (nodo->izquierdo != nullptr) {
        dot << "    \"" << p.codigo << "\" -> \"" << nodo->izquierdo->dato.codigo << "\";\n";
        escribirNodoArbol(nodo->izquierdo, dot);
    }
    if (nodo->derecho != nullptr) {
        dot << "    \"" << p.codigo << "\" -> \"" << nodo->derecho->dato.codigo << "\";\n";
        escribirNodoArbol(nodo->derecho, dot);
    }
}

void generarReporteCartelera(const ArbolBinarioPeliculas& arbol, const std::string& carpetaReportes) {
    std::ostringstream dot;
    dot << "digraph Cartelera {\n";
    dot << "    rankdir=TB;\n";
    dot << "    node [shape=box, style=filled, fontname=\"Arial\", fontsize=10];\n";

    if (arbol.getRaiz() != nullptr) {
        escribirNodoArbol(arbol.getRaiz(), dot);
    } else {
        dot << "    vacio [label=\"Cartelera vacia\", shape=plaintext];\n";
    }

    dot << "}\n";

    ejecutarDot(dot.str(), carpetaReportes + "/reporte1_cartelera_bst");
}

// ===================== Reporte 2: Solicitudes pendientes =====================

void generarReporteSolicitudes(const ListaCircularSolicitudes& solicitudes, const std::string& carpetaReportes) {
    std::ostringstream dot;
    dot << "digraph Solicitudes {\n";
    dot << "    rankdir=LR;\n";
    dot << "    node [shape=circle, style=filled, fillcolor=\"#FFF3B0\", fontname=\"Arial\", fontsize=9];\n";

    // Reunimos solo las solicitudes en estado "Pendiente".
    std::vector<int> numerosPendientes;
    NodoSolicitud* primero = solicitudes.getPrimero();

    if (primero != nullptr) {
        NodoSolicitud* actual = primero;
        int totalNodos = solicitudes.getTamanio();
        for (int i = 0; i < totalNodos; i++) {
            if (actual->dato.estado == "Pendiente") {
                const Solicitud& s = actual->dato;
                dot << "    \"S" << s.numero << "\" [label=\"#" << s.numero
                    << "\\nCliente: " << escapar(s.nombreCliente)
                    << "\\nTipo: " << s.tipoSolicitud
                    << "\\nEstado: " << s.estado << "\"];\n";
                numerosPendientes.push_back(s.numero);
            }
            actual = actual->siguiente;
        }
    }

    // Conectamos las pendientes en un ciclo bidireccional, para
    // representar visualmente el caracter circular de la lista.
    for (size_t i = 0; i < numerosPendientes.size(); i++) {
        int actualNum = numerosPendientes[i];
        int siguienteNum = numerosPendientes[(i + 1) % numerosPendientes.size()];
        if (numerosPendientes.size() > 1) {
            dot << "    \"S" << actualNum << "\" -> \"S" << siguienteNum << "\" [dir=both];\n";
        }
    }

    dot << "    label=\"Total de solicitudes pendientes: " << numerosPendientes.size() << "\";\n";
    dot << "    labelloc=\"t\";\n";
    dot << "}\n";

    ejecutarDot(dot.str(), carpetaReportes + "/reporte2_solicitudes_pendientes");
}

// ===================== Reporte 3: Promociones y beneficios =====================

void generarReportePromociones(const ListaCircularPromociones& promociones, const std::string& carpetaReportes) {
    std::ostringstream dot;
    dot << "digraph Promociones {\n";
    dot << "    rankdir=TB;\n";
    dot << "    node [fontname=\"Arial\", fontsize=9];\n";

    NodoPromocion* primero = promociones.getPrimero();

    if (primero != nullptr) {
        int total = promociones.getTamanio();
        NodoPromocion* actual = primero;

        // Fila superior: las promociones, todas en el mismo "rank"
        // para que Graphviz las dibuje alineadas horizontalmente.
        dot << "    { rank=same; ";
        for (int i = 0; i < total; i++) {
            dot << "\"" << actual->dato.codigo << "\"; ";
            actual = actual->siguiente;
        }
        dot << "}\n";

        actual = primero;
        for (int i = 0; i < total; i++) {
            const Promocion& p = actual->dato;
            dot << "    \"" << p.codigo << "\" [shape=box, style=filled, fillcolor=\"#B7E4C7\", "
                << "label=\"" << p.codigo << "\\n" << escapar(p.nombre)
                << "\\nVigencia: " << p.fechaInicio << " a " << p.fechaFin << "\"];\n";

            // Cadena horizontal entre promociones consecutivas.
            NodoPromocion* siguientePromo = actual->siguiente;
            dot << "    \"" << p.codigo << "\" -> \"" << siguientePromo->dato.codigo << "\";\n";

            // Cadena vertical de beneficios de esta promocion.
            NodoBeneficio* beneficioActual = p.beneficios.getCabeza();
            std::string idAnterior = "\"" + p.codigo + "\"";
            int contadorBeneficio = 0;
            while (beneficioActual != nullptr) {
                std::string idBeneficio = "\"" + p.codigo + "_b" + std::to_string(contadorBeneficio) + "\"";
                dot << "    " << idBeneficio << " [shape=box, style=\"filled,dashed\", fillcolor=\"#FFF3B0\", "
                    << "label=\"Tipo: " << beneficioActual->dato.tipo
                    << "\\n" << escapar(beneficioActual->dato.descripcion)
                    << "\\nValor: " << beneficioActual->dato.valor << "\"];\n";
                dot << "    " << idAnterior << " -> " << idBeneficio << " [style=dashed];\n";

                idAnterior = idBeneficio;
                beneficioActual = beneficioActual->siguiente;
                contadorBeneficio++;
            }

            actual = actual->siguiente;
        }
    } else {
        dot << "    vacio [label=\"No hay promociones registradas\", shape=plaintext];\n";
    }

    dot << "}\n";

    ejecutarDot(dot.str(), carpetaReportes + "/reporte3_promociones_beneficios");
}

// ===================== Reporte 4: Matriz de asientos =====================

void generarReporteMatrizAsientos(const MatrizDispersaAsientos& matriz, const std::string& carpetaReportes) {
    std::ostringstream dot;
    dot << "digraph MatrizAsientos {\n";
    dot << "    rankdir=TB;\n";
    dot << "    node [fontname=\"Arial\", fontsize=9];\n";

    if (!matriz.existeFuncion()) {
        dot << "    vacio [label=\"No hay ninguna funcion creada\", shape=plaintext];\n";
        dot << "}\n";
        ejecutarDot(dot.str(), carpetaReportes + "/reporte4_matriz_asientos");
        return;
    }

    int filas = matriz.getNumFilas();
    int columnas = matriz.getNumColumnas();

    dot << "    \"Funcion\" [shape=diamond, style=filled, fillcolor=\"#CDEAC0\", "
        << "label=\"Funcion:\\n" << escapar(matriz.getPelicula())
        << " - " << matriz.getHorario() << "\\n" << escapar(matriz.getSala()) << "\"];\n";

    // Cabeceras de columna, encadenadas horizontalmente desde "Funcion".
    dot << "    { rank=same; ";
    for (int c = 1; c <= columnas; c++) {
        dot << "\"col" << c << "\"; ";
    }
    dot << "}\n";
    for (int c = 1; c <= columnas; c++) {
        dot << "    \"col" << c << "\" [shape=box, style=filled, fillcolor=\"#FFE066\", label=\"Col " << c << "\"];\n";
    }
    dot << "    \"Funcion\" -> \"col1\";\n";
    for (int c = 1; c < columnas; c++) {
        dot << "    \"col" << c << "\" -> \"col" << (c + 1) << "\";\n";
    }

    // Cabeceras de fila, encadenadas verticalmente desde "Funcion".
    for (int f = 1; f <= filas; f++) {
        dot << "    \"fila" << f << "\" [shape=box, style=filled, fillcolor=\"#FFE066\", label=\"Fila " << f << "\"];\n";
    }
    dot << "    \"Funcion\" -> \"fila1\";\n";
    for (int f = 1; f < filas; f++) {
        dot << "    \"fila" << f << "\" -> \"fila" << (f + 1) << "\";\n";
    }

    // Nodos de valor (asientos reservados), conectados a su fila y columna.
    for (int f = 1; f <= filas; f++) {
        NodoAsiento* actual = matriz.getCabeceraFila(f);
        std::string idAnteriorFila = "\"fila" + std::to_string(f) + "\"";
        while (actual != nullptr) {
            std::string idValor = "\"asiento_" + std::to_string(actual->fila) + "_" + std::to_string(actual->columna) + "\"";
            dot << "    " << idValor << " [shape=circle, style=filled, fillcolor=\"#F4A6A6\", label=\""
                << escapar(actual->nombreCliente) << "\"];\n";

            dot << "    " << idAnteriorFila << " -> " << idValor << " [dir=both];\n";
            dot << "    \"col" << actual->columna << "\" -> " << idValor << " [dir=both];\n";

            idAnteriorFila = idValor;
            actual = actual->siguienteEnFila;
        }
    }

    dot << "    labelloc=\"b\";\n";
    dot << "    label=\"Reservados: " << matriz.getTotalReservados()
        << " | Libres: " << matriz.getTotalLibres()
        << " | Total: " << matriz.getTotalAsientos() << "\";\n";

    dot << "}\n";

    ejecutarDot(dot.str(), carpetaReportes + "/reporte4_matriz_asientos");
}

} // namespace ReportesGraphviz