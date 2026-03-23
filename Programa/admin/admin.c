#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../security/hash.h"
#include "admin.h"

// memoria dinámica
Usuario *listaUsuarios = NULL;
int cantidadUsuarios = 0;

void cargarUsuarios() {

    FILE *archivo = fopen(ARCHIVO_USUARIOS, "r");

    if (!archivo) {
        printf("No existe archivo, creando...\n");
        archivo = fopen(ARCHIVO_USUARIOS, "w");
        fclose(archivo);
        return;
    }

    Usuario temp;

    while (fscanf(archivo, " %49[^,],%lu", temp.usuario, &temp.hash) == 2) {

        cantidadUsuarios++;

        listaUsuarios = realloc(listaUsuarios, cantidadUsuarios * sizeof(Usuario));

        listaUsuarios[cantidadUsuarios - 1] = temp;
    }

    fclose(archivo);
}

int IniciarSesionAdmin() {

    char usuarioIngresado[50];
    char contrasenaIngresada[50];

    printf("Usuario: ");
    scanf("%49s", usuarioIngresado);

    printf("Contrasena: ");
    scanf("%49s", contrasenaIngresada);

    unsigned long hashIngresado = hashContrasena(contrasenaIngresada);

    for (int i = 0; i < cantidadUsuarios; i++) {

        if (strcmp(usuarioIngresado, listaUsuarios[i].usuario) == 0) {

            if (hashIngresado == listaUsuarios[i].hash) {
                return 1;
            } else {
                return 0;
            }
        }
    }

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


void liberarUsuarios() {
    free(listaUsuarios);
    listaUsuarios = NULL;
    cantidadUsuarios = 0;
}