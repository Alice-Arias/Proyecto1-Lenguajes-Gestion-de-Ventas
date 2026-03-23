#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../security/hash.h"
#include "admin.h"


int IniciarSesionAdmin() {
    char usuarioIngresado[50];
    char contrasenaIngresada[50];

    char usuarioArchivo[50];
    unsigned long hashGuardado;

    printf("Usuario: ");
    scanf("%49s", usuarioIngresado);

    printf("Contrasena: ");
    scanf("%49s", contrasenaIngresada);

    FILE *archivoUsuarios = fopen("data/usuarios.txt", "r");

    if (!archivoUsuarios) {
        printf("Error al abrir archivo\n");
        return 0;
    }

    unsigned long hashIngresado = hashContrasena(contrasenaIngresada);

    while (fscanf(archivoUsuarios, " %49[^,],%lu", usuarioArchivo, &hashGuardado) == 2) {// Lee usuario y hash del archivo

        if (strcmp(usuarioIngresado, usuarioArchivo) == 0) {

            fclose(archivoUsuarios);

            if (hashIngresado == hashGuardado) {
                return 1;
            } else {
                return 0;
            }
        }
    }

    fclose(archivoUsuarios);
    printf("Usuario no encontrado\n");
    return 0;
}

void menuAdmin() {
    int opcion;

    do {
        printf("\n========== MENU ADMINISTRADOR ==========\n");
        printf("1. Gestion de Sitios de Eventos\n");
        printf("2. Gestion de espacios sitio de eventos\n");
        printf("3. Gestion de Eventos\n");
        printf("4. Estado de evento\n");
        printf("5. Lista de facturas\n");
        printf("6. Estadisticas\n");
        printf("7. Volver\n");
        printf("=========================================\n");

        printf("Seleccione: ");
        scanf("%d", &opcion);

        switch(opcion) {
            case 1:
                printf("Aqui van sitios...\n");
                break;
            case 2:
                printf("Aqui van espacios...\n");
                break;
            case 3:
                printf("Aqui van eventos...\n");
                break;
            case 4:
                printf("Aqui va estado de eventos...\n");
                break;
            case 5:
                printf("Aqui va lista de facturas...\n");
                break;
            case 6:
                printf("Aqui van estadisticas...\n");
                break;
            case 7:
                printf("Volviendo...\n");
                break;
            default:
                printf("Opcion invalida\n");
        }

    } while(opcion != 7);
}