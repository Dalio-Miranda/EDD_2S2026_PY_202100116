#ifndef LZW_H
#define LZW_H

#include <string>
#include "ArregloDinamico.h"

// Implementacion manual del algoritmo de compresion Lempel-Ziv-Welch
// (LZW), sin usar ninguna libreria de compresion externa. Se usa
// para la persistencia de sedes.edd, peliculas.edd y funciones.edd
// (enunciado Fase 3, seccion "Compresion").
//
// Formato de salida: cada codigo del diccionario ocupa exactamente 2
// bytes (16 bits, big-endian), lo que permite hasta 65536 entradas
// de diccionario -- mas que suficiente para el tamanio de texto que
// maneja esta aplicacion (la serializacion de sedes/peliculas/
// funciones nunca llega a ese volumen). Se eligio ancho fijo (en vez
// de codigos de ancho variable, como usan algunas implementaciones
// mas sofisticadas de LZW) porque es mucho mas simple de implementar
// correctamente sin introducir bugs de empaquetado de bits, y de
// todas formas logra compresion real cuando el texto de entrada
// tiene patrones repetidos (como el JSON de estas 3 estructuras).
class LZW {
public:
    // Comprime el texto de entrada. Devuelve la secuencia de bytes
    // comprimidos (lista para escribir directamente a un archivo
    // .edd). Con texto vacio, devuelve un arreglo vacio.
    static ArregloDinamico<unsigned char> comprimir(const std::string& textoOriginal);

    // Proceso inverso: reconstruye el texto original exacto a partir
    // de los bytes comprimidos (los mismos que devuelve comprimir()).
    static std::string descomprimir(const ArregloDinamico<unsigned char>& datosComprimidos);
};

#endif // LZW_H
