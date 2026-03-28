#include "hash.h"

/*
 * Objetivo: Calcular un hash tipo djb2 para una cadena de texto.
 * Entradas: str, puntero a cadena terminada en '\0'.
 * Salidas: Valor hash sin signo de tipo unsigned long.
 * Restricciones: str debe ser un puntero valido (no NULL).
 */
unsigned long hashContrasena(char *str) {
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }

    return hash;
}