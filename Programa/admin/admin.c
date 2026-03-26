#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../security/hash.h"
#include "admin.h"
#include "../include/sitio.h"
#include "../include/colors.h"
#include "../include/evento.h"


Usuario *listaUsuarios = NULL;
int cantidadUsuarios = 0;


void cargarUsuarios() {

    FILE *archivo = fopen(ARCHIVO_USUARIOS, "r");

    if (!archivo) {
        printf(MSG_INFO "No existe archivo, creando...\n" RESET);
        archivo = fopen(ARCHIVO_USUARIOS, "w");
        fclose(archivo);
        return;
    }

    Usuario temp;

    while (fscanf(archivo, " %49[^,],%lu", temp.usuario, &temp.hash) == 2) {

        cantidadUsuarios++;

        Usuario *tempPtr = realloc(listaUsuarios, cantidadUsuarios * sizeof(Usuario));
        if (tempPtr == NULL) {
            printf(MSG_ERROR "Error en memoria\n" RESET);
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

    printf(MENU_INPUT "Usuario: " RESET);
    scanf("%49s", usuarioIngresado);

    printf(MENU_INPUT "Contrasena: " RESET);
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

    printf(MSG_ERROR "Usuario no encontrado\n" RESET);
    return 0;
}

void menuEventos() {
    int opcion;

    do {
        printf("\n" MENU_BORDER "====================================\n" RESET);
        printf(MENU_TITLE "      GESTION DE EVENTOS\n" RESET);
        printf(MENU_BORDER "====================================\n" RESET);

        printf(MENU_OPTION "1. Crear evento\n" RESET);
        printf(MENU_OPTION "2. Mostrar eventos\n" RESET);
        printf(MENU_OPTION "3. Volver\n" RESET);

        printf(MENU_BORDER "====================================\n" RESET);

        printf(MENU_INPUT "Seleccione: " RESET);
        scanf("%d", &opcion);

        switch(opcion) {

            case 1: {
                char nombre[50], productora[50], fecha[20];
                int indiceSitio;

                printf(MENU_INPUT "Nombre del evento: " RESET);
                scanf(" %[^\n]", nombre);

                printf(MENU_INPUT "Productora: " RESET);
                scanf(" %[^\n]", productora);

                printf(MENU_INPUT "Fecha: " RESET);
                scanf(" %[^\n]", fecha);

                mostrarSitios();
                printf(MENU_INPUT "Seleccione sitio: " RESET);
                scanf("%d", &indiceSitio);

                if (indiceSitio < 1 || indiceSitio > cantidadSitios) {
                    printf(MSG_ERROR "Sitio invalido\n" RESET);
                    break;
                }

                SitioEvento *sitio = &listaSitios[indiceSitio - 1];

                mostrarSectoresDeSitio(sitio);

                int numSectores = sitio->totalSectores;

                if (numSectores == 0) {
                    printf(MSG_ERROR "El sitio no tiene sectores.\n" RESET);
                    break;
                }

                float *precios = malloc(numSectores * sizeof(float));
                if (!precios) {
                    printf(MSG_ERROR "Error de memoria\n" RESET);
                    break;
                }

                for (int i = 0; i < numSectores; i++) {
                    printf(MENU_INPUT "Precio para sector '%s': " RESET, sitio->sectores[i].nombre);
                    scanf("%f", &precios[i]);
                }

                crearEvento(nombre, productora, fecha, sitio, precios);

                free(precios);
                break;
            }

            case 2:
                mostrarEventos();
                break;

            case 3:
                printf(MSG_INFO "Volviendo...\n" RESET);
                break;

            default:
                printf(MSG_ERROR "Opcion invalida\n" RESET);
        }

    } while(opcion != 3);
}

void menuSitios() {
    int opcion;
    char ruta[200];

    do {
        printf("\n" MENU_BORDER "====================================\n" RESET);
        printf(MENU_TITLE "      GESTION DE SITIOS\n" RESET);
        printf(MENU_BORDER "====================================\n" RESET);

        printf(MENU_OPTION "1. Cargar sitios desde archivo\n" RESET);
        printf(MENU_OPTION "2. Agregar sitio manualmente\n" RESET);
        printf(MENU_OPTION "3. Mostrar sitios\n" RESET);
        printf(MENU_OPTION "4. Guardar sitios\n" RESET);
        printf(MENU_OPTION "5. Volver\n" RESET);

        printf(MENU_BORDER "====================================\n" RESET);

        printf(MENU_INPUT "Seleccione: " RESET);
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
                    printf(MSG_ERROR "Datos invalidos.\n" RESET);
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
        printf("\n" MENU_BORDER "====================================\n" RESET);
        printf(MENU_TITLE "      GESTION DE ESPACIOS\n" RESET);
        printf(MENU_BORDER "====================================\n" RESET);

        printf(MENU_OPTION "1. Ver sectores de un sitio\n" RESET);
        printf(MENU_OPTION "2. Agregar sector a un sitio\n" RESET);
        printf(MENU_OPTION "3. Resetear sectores de un sitio\n" RESET);
        printf(MENU_OPTION "4. Volver\n" RESET);

        printf(MENU_BORDER "====================================\n" RESET);

        printf(MENU_INPUT "Seleccione: " RESET);
        scanf("%d", &opcion);

        switch(opcion) {

            case 1:
                mostrarSitios();
                printf("Seleccione sitio: ");
                scanf("%d", &indiceSitio);

                if (indiceSitio < 1 || indiceSitio > cantidadSitios) {
                    printf(MSG_ERROR "Sitio invalido\n" RESET);
                    break;
                }

                mostrarSectoresDeSitio(&listaSitios[indiceSitio - 1]);
                break;

            case 2:
                mostrarSitios(); 
                printf("Seleccione sitio: ");
                scanf("%d", &indiceSitio);

                if (indiceSitio < 1 || indiceSitio > cantidadSitios) {
                    printf(MSG_ERROR "Sitio invalido\n" RESET);
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
                    printf(MSG_ERROR "Sitio invalido\n" RESET);
                    break;
                }

                resetSectoresDeSitio(&listaSitios[indiceSitio - 1]);
                break;

            case 4:
                printf("Volviendo...\n");
                break;

            default:
                printf(MSG_ERROR "Opcion invalida\n" RESET);
        }

    } while(opcion != 4);
}

void menuAdmin() {
    int opcion;

    do {
        printf("\n" MENU_BORDER "=========================================\n" RESET);
        printf(MENU_TITLE "        MENU ADMINISTRADOR\n" RESET);
        printf(MENU_BORDER "=========================================\n" RESET);

        printf(MENU_OPTION "1. Gestion de Sitios de Eventos\n" RESET);
        printf(MENU_OPTION "2. Gestion de espacios sitio de eventos\n" RESET);
        printf(MENU_OPTION "3. Gestion de Eventos\n" RESET);
        printf(MENU_OPTION "4. Estado de evento\n" RESET);
        printf(MENU_OPTION "5. Lista de facturas\n" RESET);
        printf(MENU_OPTION "6. Estadisticas\n" RESET);
        printf(MENU_OPTION "7. Volver\n" RESET);

        printf(MENU_BORDER "=========================================\n" RESET);

        printf(MENU_INPUT "Seleccione: " RESET);
        scanf("%d", &opcion);

        switch(opcion) {
            case 1:
                menuSitios();
                break;

            case 2:
                menuSectores();
                break;

            case 3:
                menuEventos();
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