#include <stdio.h>
#include "sistema.h"

void inicializarSistema() {
    FILE *archivo;

    archivo = fopen("data/usuarios.txt", "r");

    if (!archivo) {
        archivo = fopen("data/usuarios.txt", "w");
        if (archivo) {
            printf("Archivo usuarios creado.\n");
        }
    }

    if (archivo) fclose(archivo);
}