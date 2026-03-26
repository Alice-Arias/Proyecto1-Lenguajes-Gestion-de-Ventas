#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../security/hash.h"
#include "admin.h"
#include "../include/sitio.h"


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

        Usuario *tempPtr = realloc(listaUsuarios, cantidadUsuarios * sizeof(Usuario));
        if (tempPtr == NULL) {
            printf("Error de memoria\n");
            return;
        }

        listaUsuarios = tempPtr;
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


void menuSitios() {
    int opcion;
    char ruta[200];

    do {
        printf("\n===== GESTION DE SITIOS =====\n");
        printf("1. Cargar sitios desde archivo\n");
        printf("2. Agregar sitio manualmente\n");
        printf("3. Mostrar sitios\n");
        printf("4. Guardar sitios\n");
        printf("5. Volver\n");
        printf("=============================\n");

        printf("Seleccione: ");
        scanf("%d", &opcion);

        switch(opcion) {

            case 1:
                printf("Ingrese ruta del archivo: ");
                scanf("%s", ruta);
                cargarSitiosDesdeArchivo(ruta);
                break;

            case 2: {
                char nombre[50], ubicacion[150], web[100];

                printf("Nombre: ");
                scanf(" %[^\n]", nombre);

                printf("Ubicacion: ");
                scanf(" %[^\n]", ubicacion);

                printf("Web (opcional): ");
                scanf(" %[^\n]", web);

                if (strlen(nombre) == 0 || strlen(ubicacion) == 0) {
                    printf("Datos invalidos.\n");
                    break;
                }

                agregarSitio(nombre, ubicacion, web);
                break;
            }

            case 3:
                mostrarSitios();
                break;

            case 4:
                guardarSitiosEnArchivo("data/sitios.txt");
                break;

            case 5:
                printf("Volviendo...\n");
                break;

            default:
                printf("Opcion invalida\n");
        }

    } while(opcion != 5);
}

void menuSectores() {
    int opcion;
    int indiceSitio;
    char nombre[50];
    char inicial;
    int cantidad;

    do {
        printf("\n===== GESTION DE ESPACIOS =====\n");
        printf("1. Ver sectores de un sitio\n");
        printf("2. Agregar sector a un sitio\n");
        printf("3. Resetear sectores de un sitio\n");
        printf("4. Volver\n");
        printf("================================\n");

        printf("Seleccione: ");
        scanf("%d", &opcion);

        switch(opcion) {

            case 1:
                mostrarSitios(); // se mantiene aquí
                printf("Seleccione sitio: ");
                scanf("%d", &indiceSitio);

                if (indiceSitio < 1 || indiceSitio > cantidadSitios) {
                    printf("Sitio invalido\n");
                    break;
                }

                mostrarSectoresDeSitio(&listaSitios[indiceSitio - 1]);
                break;

            case 2:
                mostrarSitios(); // necesario
                printf("Seleccione sitio: ");
                scanf("%d", &indiceSitio);

                if (indiceSitio < 1 || indiceSitio > cantidadSitios) {
                    printf("Sitio invalido\n");
                    break;
                }

                printf("Nombre del sector: ");
                scanf(" %[^\n]", nombre);

                printf("Inicial (letra): ");
                scanf(" %c", &inicial);

                printf("Cantidad de asientos: ");
                scanf("%d", &cantidad);

                agregarSectorASitio(
                    &listaSitios[indiceSitio - 1],
                    nombre,
                    inicial,
                    cantidad
                );
                break;

            case 3:
                mostrarSitios();
                printf("Seleccione sitio: ");
                scanf("%d", &indiceSitio);

                if (indiceSitio < 1 || indiceSitio > cantidadSitios) {
                    printf("Sitio invalido\n");
                    break;
                }

                resetSectoresDeSitio(&listaSitios[indiceSitio - 1]);
                break;

            case 4:
                printf("Volviendo...\n");
                break;

            default:
                printf("Opcion invalida\n");
        }

    } while(opcion != 4);
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
                menuSitios();
                break;

            case 2:
                menuSectores();
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