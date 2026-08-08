#ifndef PELICULA_H
#define PELICULA_H

#include <string>

// Representa una película registrada en la cartelera.
// Es un struct simple (sin comportamiento propio) porque
// la lógica de la lista la maneja ListaDoblePeliculas.
struct Pelicula {
    std::string codigo;         // Ej: "P001"
    std::string titulo;
    std::string genero;         // Accion, Comedia, Drama, Terror, etc.
    int duracion = 0;           // en minutos
    std::string clasificacion;  // "A", "AA", "B15", "C"
    std::string idioma;         // Español, Subtitulada, Doblada
    std::string fechaEstreno;   // formato AAAA-MM-DD
    std::string fechaFin;       // formato AAAA-MM-DD (fin de cartelera)
};

#endif // PELICULA_H
