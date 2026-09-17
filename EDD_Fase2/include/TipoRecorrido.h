#ifndef TIPO_RECORRIDO_H
#define TIPO_RECORRIDO_H

// Tipo de recorrido para listar el contenido de un arbol. Compartido
// entre ArbolBinarioPeliculas, ArbolAVLFunciones y ArbolBClientes
// para que las 3 estructuras ofrezcan los mismos 3 recorridos que
// pide el enunciado (preorden, inorden, postorden).
enum class TipoRecorrido { PREORDEN, INORDEN, POSTORDEN };

#endif // TIPO_RECORRIDO_H
