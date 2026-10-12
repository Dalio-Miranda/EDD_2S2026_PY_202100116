#ifndef BLOCKCHAIN_H
#define BLOCKCHAIN_H

#include <string>
#include "ArregloDinamico.h"
#include "Bloque.h"
#include "Transaccion.h"
#include "TablaHashReservas.h"

// Cada cuantas transacciones pendientes se genera un bloque
// automaticamente si el administrador no lo hace manualmente antes.
constexpr int TRANSACCIONES_POR_BLOQUE_AUTOMATICO = 4;

// Nodo de la blockchain: los bloques se comportan como una lista
// enlazada SIMPLE, insertados por delante (enunciado Fase 3,
// "Seguridad"). "siguiente" apunta hacia el bloque anterior en la
// cadena (mas viejo); la cabeza de la lista es siempre el bloque MAS
// RECIENTE.
struct NodoBloque {
    Bloque dato;
    NodoBloque* siguiente = nullptr;
};

// La blockchain completa: administra la cadena de bloques, la cola
// de transacciones pendientes (en memoria, se pierden si se cierra
// el programa sin minar un bloque -- el enunciado las describe como
// una "cola/lista/pila temporal en memoria"), la persistencia en
// blocks/*.json, y la verificacion de integridad al cargar.
class Blockchain {
private:
    NodoBloque* cabeza = nullptr; // bloque mas reciente
    int totalBloques = 0;
    std::string carpetaBlocks;

    ArregloDinamico<Transaccion> pendientes;

    // Estado de la cadena TAL COMO ESTA EN DISCO (blocks/): cada bloque
    // leido, mas si su integridad propia es valida. Sirve para el
    // reporte verde/rojo (enunciado: "Proteccion de datos"): un bloque
    // manipulado a mano en su JSON aparece en rojo aunque no se cargue
    // a memoria. Se mantiene al dia cuando se generan bloques nuevos.
    ArregloDinamico<Bloque> cadenaEnDisco;
    ArregloDinamico<bool> cadenaEnDiscoIntegra;

    void insertarAlFrente(const Bloque& b);
    void registrarEnCadenaDisco(const Bloque& b, bool integro);
    void persistirBloque(const Bloque& b) const;
    void liberarCadena();

public:
    explicit Blockchain(std::string carpetaBlocks = "blocks");
    ~Blockchain();

    Blockchain(const Blockchain&) = delete;
    Blockchain& operator=(const Blockchain&) = delete;

    // Resultado de leer la blockchain desde disco al iniciar el
    // programa.
    struct ResultadoCarga {
        int bloquesValidos = 0;
        int bloquesDescartados = 0;
        bool huboManipulacion = false;
        int indiceDelPrimerManipulado = -1;
    };

    // Lee todos los bloques de carpetaBlocks/ (block0.json,
    // block1.json, ...), verifica la integridad de la cadena
    // COMPLETA en orden (genesis primero): el hash de cada bloque
    // debe coincidir con el recalculado, su PREVIOUSHASH debe
    // coincidir con el hash real del bloque anterior, y su
    // ROOTMERKLE debe coincidir con la raiz de Merkle recalculada a
    // partir de su DATA actual. Se detiene en el primer bloque que
    // falle cualquiera de esas 3 verificaciones -- SOLO los bloques
    // validos anteriores a ese punto se cargan a memoria (enunciado:
    // "Se cargara al sistema unicamente el ultimo bloque no
    // manipulado de la lista").
    ResultadoCarga cargarDesdeDisco();

    // Agrega una compra a la cola de pendientes. Si al agregarla se
    // alcanzan TRANSACCIONES_POR_BLOQUE_AUTOMATICO pendientes, genera
    // un bloque automaticamente (sin que el administrador tenga que
    // pedirlo).
    void agregarTransaccionPendiente(const Transaccion& t);

    int getCantidadPendientes() const;

    // Genera un bloque manualmente con las transacciones pendientes
    // actuales (las vacia despues de minarlo). No hace nada y
    // retorna false si no hay ninguna pendiente.
    bool generarBloque();

    // Verificacion de integridad PROPIA de un bloque (sin mirar a su
    // vecino): el HASH almacenado coincide con el recalculado, y el
    // ROOTMERKLE almacenado coincide con la raiz de Merkle recalculada
    // a partir de su DATA actual.
    static bool bloqueEsIntegro(const Bloque& b);

    // Genera un codigo de transaccion que no exista todavia ni en la
    // cadena ni en la cola de pendientes (formato T001, T002, ...).
    std::string generarCodigoTransaccion() const;

    ArregloDinamico<Transaccion> getPendientes() const;

    // Estado de blocks/ tal como se leyo/escribio (ver comentario de
    // los miembros). Ambos arreglos tienen siempre el mismo tamanio.
    const ArregloDinamico<Bloque>& getCadenaEnDisco() const;
    const ArregloDinamico<bool>& getCadenaEnDiscoIntegra() const;

    bool hayBloques() const;
    int getCantidadBloques() const;
    Bloque* getUltimoBloque() const;
    NodoBloque* getCabeza() const;

    // Recorre el DATA de TODOS los bloques validos (del mas viejo al
    // mas nuevo) e inserta cada transaccion en la Tabla Hash como una
    // Reserva. Es la unica forma en que la Tabla Hash de reservas se
    // reconstruye al iniciar el programa (enunciado: "la Tabla Hash
    // no depende de ningun archivo adicional; su contenido siempre
    // se deriva de la blockchain").
    void reconstruirTablaHash(TablaHashReservas& tabla) const;

    // Todas las transacciones de un cliente especifico, buscando en
    // el DATA de TODOS los bloques (no solo el ultimo).
    ArregloDinamico<Transaccion> historialDeCliente(const std::string& idCliente) const;

    // Comprobante de una compra especifica: en que bloque quedo
    // registrada (indice + hash de ese bloque), ademas de la
    // transaccion completa.
    struct Comprobante {
        bool encontrado = false;
        Transaccion transaccion;
        int indiceBloque = -1;
        std::string hashBloque;
    };
    Comprobante buscarTransaccion(const std::string& codigoTransaccion) const;

    // Resumen de ventas totales, recorriendo el DATA de TODOS los
    // bloques validos de la cadena.
    struct ResumenVentas {
        int totalBoletos = 0;
        double montoTotal = 0.0;
    };
    ResumenVentas calcularVentasTotales() const;

    // Desglose de las ventas por una clave (codigo de funcion o fecha
    // de pago), recorriendo el DATA de todos los bloques.
    struct LineaDesglose {
        std::string clave;
        int boletos = 0;
        double monto = 0.0;
    };
    ArregloDinamico<LineaDesglose> desglosePorFuncion() const;
    ArregloDinamico<LineaDesglose> desglosePorFecha() const;

private:
    ArregloDinamico<LineaDesglose> desglosar(bool porFuncion) const;
public:
};

#endif // BLOCKCHAIN_H
