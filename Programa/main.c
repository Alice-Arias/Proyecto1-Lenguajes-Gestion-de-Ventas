#include <stdio.h>
#include <stdlib.h>
#include "security/hash.h"
#include <string.h>

void crearUsuario();

int main() {
    int opcion;

    do {
        printf("\n=== Menu ===\n");
        printf("1. Crear usuario\n");
        printf("2. Salir\n");
        printf("Seleccione: ");
        scanf("%d", &opcion);

        switch(opcion) {
            case 1:
                crearUsuario();
                break;
            case 2:
                printf("Saliendo.\n");
                break;
            default:
                printf("Opcion invalida\n");
        }

    } while(opcion != 2);

    return 0;
}

void crearUsuario() {
    char user[50], pass[50];

    printf("Nuevo usuario: ");
    scanf("%s", user);

    printf("Contrasena: ");
    scanf("%s", pass);

    FILE *archivo = fopen("data/usuarios.txt", "a");

    if (!archivo) {
        printf("Error al abrir archivo\n");
        return;
    }

    unsigned long hash = hashContrasena(pass);

    fprintf(archivo, "%s,%lu\n", user, hash);// Guardar el usuario y la contraseña encriptada

    fclose(archivo);

    printf("Usuario guardado (encriptado).\n");
}