#include <stdio.h>
#include "sistema.h"
#include "../include/colors.h"

void inicializarSistema() {
    FILE *archivo;

    archivo = fopen("data/usuarios.txt", "r");

    if (!archivo) {
        archivo = fopen("data/usuarios.txt", "w");
        if (archivo) {
            printf(MSG_INFO "Archivo usuarios creado.\n" RESET);
        }
    }

    if (archivo) fclose(archivo);
}