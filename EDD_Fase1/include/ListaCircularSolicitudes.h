#ifndef LISTA_CIRCULAR_SOLICITUDES_H
#define LISTA_CIRCULAR_SOLICITUDES_H

#include <string>
#include <vector>
#include "Solicitud.h"

// Nodo de la lista circular DOBLEMENTE enlazada de solicitudes.
// A diferencia de NodoPromocion (lista circular SIMPLE), aquí sí
// llevamos "anterior" además de "siguiente", porque el enunciado
// requiere poder recorrer las solicitudes en ambos sentidos
// (por ejemplo, para navegar hacia atrás en la cola de atención).
struct NodoSolicitud {
    Solicitud dato;
    NodoSolicitud* anterior;
    NodoSolicitud* siguiente;

    NodoSolicitud(const Solicitud& s)
        : dato(s), anterior(nullptr), siguiente(nullptr) {}
};

// Lista circular doblemente enlazada que gestiona las solicitudes
// especiales de los clientes. El administrador puede ver las
// pendientes, aprobarlas (cambiar estado) o rechazarlas (eliminarlas).
class ListaCircularSolicitudes {
private:
    NodoSolicitud* ultimo;   // guardamos el último; ultimo->siguiente es el primero
    int tamanio;
    int siguienteNumero;     // contador para autogenerar el numero de solicitud

public:
    ListaCircularSolicitudes();
    ~ListaCircularSolicitudes();

    // Registra una nueva solicitud (estado inicial "Pendiente",
    // numero y fecha autogenerados). Retorna el numero asignado.
    int registrarSolicitud(const std::string& nombreCliente,
                            const std::string& telefono,
                            const std::string& tipoSolicitud,
                            const std::string& descripcion);

    // Cambia el estado de una solicitud existente (para aprobar).
    bool cambiarEstado(int numero, const std::string& nuevoEstado);

    // Elimina la solicitud de la lista (usado para rechazarla).
    bool eliminar(int numero);

    Solicitud* buscarPorNumero(int numero);

    // Un cliente puede tener varias solicitudes con el mismo telefono.
    std::vector<Solicitud> buscarPorTelefono(const std::string& telefono);

    int contarPendientes() const;

    void imprimirPendientes() const;
    void imprimir() const; // todas, sin filtrar por estado

    int getTamanio() const;
    bool estaVacia() const;
    NodoSolicitud* getPrimero() const;

private:
    static std::string obtenerFechaActual();
};

#endif // LISTA_CIRCULAR_SOLICITUDES_H