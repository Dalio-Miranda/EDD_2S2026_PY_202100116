#include "LZW.h"

namespace {

// ---------------------------------------------------------------------
// Diccionario auxiliar SOLO para la fase de compresion: mapea texto
// (string) -> codigo (int). Se implementa como una tabla hash manual
// con encadenamiento separado (mismo estilo que TablaHashReservas de
// Fase 2), para que buscar "¿el diccionario ya tiene esta cadena?"
// sea O(1) en promedio en vez de una busqueda lineal -- importante
// para que comprimir un archivo de tamanio realista no sea lento.
// ---------------------------------------------------------------------
struct NodoDiccionarioLZW {
    std::string clave;
    int codigo;
    NodoDiccionarioLZW* siguiente;
};

class DiccionarioCompresion {
private:
    static const int TAMANIO_TABLA = 4099; // numero primo
    NodoDiccionarioLZW* buckets[TAMANIO_TABLA];

    unsigned long hash(const std::string& s) const {
        unsigned long h = 5381;
        for (unsigned char c : s) {
            h = h * 33 + c;
        }
        return h % TAMANIO_TABLA;
    }

public:
    DiccionarioCompresion() {
        for (int i = 0; i < TAMANIO_TABLA; i++) buckets[i] = nullptr;
    }

    ~DiccionarioCompresion() {
        for (int i = 0; i < TAMANIO_TABLA; i++) {
            NodoDiccionarioLZW* actual = buckets[i];
            while (actual != nullptr) {
                NodoDiccionarioLZW* siguiente = actual->siguiente;
                delete actual;
                actual = siguiente;
            }
        }
    }

    void insertar(const std::string& clave, int codigo) {
        unsigned long idx = hash(clave);
        NodoDiccionarioLZW* nuevo = new NodoDiccionarioLZW{clave, codigo, buckets[idx]};
        buckets[idx] = nuevo;
    }

    // Retorna el codigo asociado, o -1 si la clave no esta en el diccionario.
    int buscar(const std::string& clave) const {
        unsigned long idx = hash(clave);
        for (NodoDiccionarioLZW* n = buckets[idx]; n != nullptr; n = n->siguiente) {
            if (n->clave == clave) return n->codigo;
        }
        return -1;
    }
};

// Empaqueta un codigo de 16 bits en 2 bytes big-endian.
void emitirCodigo(ArregloDinamico<unsigned char>& salida, int codigo) {
    salida.agregarFinal(static_cast<unsigned char>((codigo >> 8) & 0xFF));
    salida.agregarFinal(static_cast<unsigned char>(codigo & 0xFF));
}

} // namespace

ArregloDinamico<unsigned char> LZW::comprimir(const std::string& textoOriginal) {
    ArregloDinamico<unsigned char> salida;
    if (textoOriginal.empty()) return salida;

    // El diccionario inicial tiene una entrada por cada posible byte
    // (0-255), con su propio valor como codigo -- el punto de
    // partida estandar de LZW.
    DiccionarioCompresion diccionario;
    for (int i = 0; i < 256; i++) {
        diccionario.insertar(std::string(1, static_cast<char>(i)), i);
    }
    int siguienteCodigo = 256;

    std::string actual(1, textoOriginal[0]);

    for (size_t i = 1; i < textoOriginal.size(); i++) {
        std::string combinado = actual + textoOriginal[i];

        if (diccionario.buscar(combinado) != -1) {
            // La cadena combinada ya existe en el diccionario: seguir
            // extendiendo el match, todavia no se emite nada.
            actual = combinado;
        } else {
            // Se rompio el match: emitir el codigo de "actual" (que
            // si esta en el diccionario), agregar la cadena nueva
            // (combinado) con un codigo nuevo, y reiniciar desde el
            // caracter que rompio el match.
            emitirCodigo(salida, diccionario.buscar(actual));

            if (siguienteCodigo < 65536) {
                diccionario.insertar(combinado, siguienteCodigo);
                siguienteCodigo++;
            }
            actual = std::string(1, textoOriginal[i]);
        }
    }

    // Queda pendiente emitir el codigo de la ultima cadena acumulada.
    emitirCodigo(salida, diccionario.buscar(actual));

    return salida;
}

std::string LZW::descomprimir(const ArregloDinamico<unsigned char>& datosComprimidos) {
    std::string salida;
    if (datosComprimidos.estaVacio()) return salida;

    // Para descomprimir, el diccionario es codigo -> texto. Como los
    // codigos se asignan de forma secuencial (0..255 primero, luego
    // 256, 257, 258...), un simple ArregloDinamico indexado
    // directamente por el codigo reproduce el mismo diccionario que
    // se construyo durante la compresion, sin necesidad de una tabla
    // hash de este lado.
    ArregloDinamico<std::string> diccionario;
    for (int i = 0; i < 256; i++) {
        diccionario.agregarFinal(std::string(1, static_cast<char>(i)));
    }

    int totalCodigos = datosComprimidos.getTamanio() / 2;
    auto leerCodigo = [&datosComprimidos](int indice) -> int {
        unsigned char alto = datosComprimidos[indice * 2];
        unsigned char bajo = datosComprimidos[indice * 2 + 1];
        return (static_cast<int>(alto) << 8) | static_cast<int>(bajo);
    };

    int codigoAnterior = leerCodigo(0);
    std::string entradaAnterior = diccionario[codigoAnterior];
    salida += entradaAnterior;

    for (int i = 1; i < totalCodigos; i++) {
        int codigoActual = leerCodigo(i);
        std::string entradaActual;

        if (codigoActual < diccionario.getTamanio()) {
            entradaActual = diccionario[codigoActual];
        } else if (codigoActual == diccionario.getTamanio()) {
            // Caso especial clasico de LZW (patron "cScSc"): el
            // codigo que se esta leyendo es el que el propio paso de
            // compresion estaba a punto de crear, todavia no existe
            // de este lado. Se reconstruye como entradaAnterior mas
            // su propio primer caracter.
            entradaActual = entradaAnterior + entradaAnterior[0];
        } else {
            // Codigo fuera de rango: datos corruptos. No deberia
            // pasar con datos generados por comprimir().
            break;
        }

        salida += entradaActual;

        if (diccionario.getTamanio() < 65536) {
            diccionario.agregarFinal(entradaAnterior + entradaActual[0]);
        }

        entradaAnterior = entradaActual;
    }

    return salida;
}
