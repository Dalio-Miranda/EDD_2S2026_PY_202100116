#include <iostream>
#include "ArbolBinarioPeliculas.h"
#include "ListaCircularPromociones.h"
#include "ListaCircularSolicitudes.h"

void probarPeliculas() {
    std::cout << "\n########## PRUEBA: CARTELERA (ARBOL BINARIO DE BUSQUEDA) ##########\n" << std::endl;

    ArbolBinarioPeliculas cartelera;

    std::cout << "Cargando cartelera desde CSV..." << std::endl;
    int cargadas = cartelera.cargarDesdeCSV("../data/peliculas_ejemplo.csv");
    std::cout << "Se cargaron " << cargadas << " peliculas.\n" << std::endl;

    cartelera.imprimirInorden();

    std::cout << "\nInsertando una nueva pelicula (P006)..." << std::endl;
    Pelicula nueva;
    nueva.codigo = "P006";
    nueva.titulo = "Zootopia 3";
    nueva.genero = "Animacion";
    nueva.duracion = 105;
    nueva.clasificacion = "A";
    nueva.idioma = "Doblada";
    nueva.fechaEstreno = "2026-08-01";
    nueva.fechaFin = "2026-09-01";
    cartelera.insertar(nueva);

    std::cout << "\nCartelera despues de insertar P006 (inorden, sigue ordenada):" << std::endl;
    cartelera.imprimirInorden();

    std::cout << "\nBuscando P003 en el arbol..." << std::endl;
    Pelicula* encontrada = cartelera.buscar("P003");
    if (encontrada != nullptr) {
        std::cout << "Encontrada: " << encontrada->titulo << std::endl;
    } else {
        std::cout << "No encontrada." << std::endl;
    }

    std::cout << "\nEliminando P002 (nodo con posible caso de 2 hijos)..." << std::endl;
    bool eliminado = cartelera.eliminar("P002");
    std::cout << (eliminado ? "Eliminada correctamente." : "No se encontro.") << std::endl;

    std::cout << "\nCartelera final (inorden):" << std::endl;
    cartelera.imprimirInorden();
}

void probarPromociones() {
    std::cout << "\n########## PRUEBA: PROMOCIONES (LISTA CIRCULAR) ##########\n" << std::endl;

    ListaCircularPromociones promociones;

    Promocion p1;
    p1.codigo = "Promo_01";
    p1.nombre = "Martes de Locura";
    p1.fechaInicio = "2026-08-01";
    p1.fechaFin = "2026-12-31";
    p1.diasAplicables = {"Martes"};
    p1.beneficios.agregar({"descuento", "Refill gratis en combo grande", "NA"});
    p1.beneficios.agregar({"descuento", "Segunda funcion con descuento", "25%"});

    Promocion p2;
    p2.codigo = "Promo_02";
    p2.nombre = "Miercoles 2x1";
    p2.fechaInicio = "2026-08-01";
    p2.fechaFin = "2026-12-31";
    p2.diasAplicables = {"Miercoles"};
    p2.beneficios.agregar({"2x1", "Entradas 2x1", "NA"});

    Promocion p3;
    p3.codigo = "Promo_03";
    p3.nombre = "Combo Familiar";
    p3.fechaInicio = "2026-08-01";
    p3.fechaFin = "2026-12-31";
    p3.diasAplicables = {"Sabado", "Domingo"};
    p3.beneficios.agregar({"combo", "2 bebidas grandes, 3 porciones de nachos", "NA"});
    p3.beneficios.agregar({"descuento", "Asientos con descuento", "10%"});

    promociones.agregar(p1);
    promociones.agregar(p2);
    promociones.agregar(p3);

    promociones.imprimir();

    std::cout << "\nDemostrando que la lista es circular (recorriendo 7 pasos con solo 3 nodos):" << std::endl;
    promociones.recorrerCiclo(7);

    std::cout << "\nBuscando Promo_02..." << std::endl;
    Promocion* encontrada = promociones.buscar("Promo_02");
    if (encontrada != nullptr) {
        std::cout << "Encontrada: " << encontrada->nombre << std::endl;
    }

    std::cout << "\nEliminando Promo_02..." << std::endl;
    promociones.eliminar("Promo_02");
    promociones.imprimir();
}

void probarSolicitudes() {
    std::cout << "\n########## PRUEBA: SOLICITUDES ESPECIALES (LISTA CIRCULAR DOBLE) ##########\n" << std::endl;

    ListaCircularSolicitudes solicitudes;

    int n1 = solicitudes.registrarSolicitud("Ale Ramirez", "5555-1111", "cumpleanos", "Mesa decorada para cumpleanos de 10 anos");
    int n2 = solicitudes.registrarSolicitud("Jose Perez", "5555-2222", "queja", "Sonido muy alto en sala 3");
    int n3 = solicitudes.registrarSolicitud("Jens Lopez", "5555-3333", "aniversario", "Sala privada para aniversario");

    std::cout << "Solicitudes registradas con numeros: " << n1 << ", " << n2 << ", " << n3 << "\n" << std::endl;

    solicitudes.imprimir();

    std::cout << "\nPendientes: " << solicitudes.contarPendientes() << std::endl;

    std::cout << "\nAprobando solicitud #" << n1 << " (En proceso)..." << std::endl;
    solicitudes.cambiarEstado(n1, "En proceso");

    std::cout << "Rechazando solicitud #" << n2 << " (se elimina de la lista)..." << std::endl;
    solicitudes.eliminar(n2);

    std::cout << "\nEstado final:" << std::endl;
    solicitudes.imprimir();

    std::cout << "\nPendientes restantes: " << solicitudes.contarPendientes() << std::endl;

    std::cout << "\nCliente consulta su solicitud por telefono (5555-3333):" << std::endl;
    auto encontradas = solicitudes.buscarPorTelefono("5555-3333");
    for (const auto& s : encontradas) {
        std::cout << "#" << s.numero << " - " << s.tipoSolicitud << " - Estado: " << s.estado << std::endl;
    }
}

int main() {
    probarPeliculas();
    probarPromociones();
    probarSolicitudes();
    return 0;
}