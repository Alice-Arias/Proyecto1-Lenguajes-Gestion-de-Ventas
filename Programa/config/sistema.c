#include <stdio.h>
#include "sistema.h"
#include "../include/colors.h"

/*
 * Objetivo: Garantizar que exista el archivo base de usuarios al iniciar el sistema.
 * Entradas: No recibe parametros; trabaja sobre la ruta fija data/usuarios.txt.
 * Salidas: Crea el archivo si no existe y muestra mensajes informativos.
 * Restricciones: Requiere permisos de lectura/escritura sobre la carpeta data.
 */
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