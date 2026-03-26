#include <stdio.h>
#include "cliente.h"
#include "../include/colors.h"
#include "../include/evento.h"
#include "../include/sitio.h"
#include "../include/sector.h"

void comprarBoletos() {
    int eventoIdx, sectorIdx, asientoIdx;

    mostrarEventos();

    printf(MENU_INPUT "Seleccione evento: " RESET);
    if (scanf("%d", &eventoIdx) != 1) {
        printf(MSG_ERROR "Entrada invalida\n" RESET);
        while(getchar() != '\n');
        return;
    }

    if (eventoIdx < 1 || eventoIdx > totalEventos) {
        printf(MSG_ERROR "Evento invalido.\n" RESET);
        return;
    }

    Evento *evento = &eventos[eventoIdx - 1];

    printf(MSG_INFO "\nSectores disponibles:\n" RESET);

    for (int i = 0; i < evento->sitio->totalSectores; i++) {
        printf("%d. %s\n", i + 1, evento->sitio->sectores[i].nombre);
    }

    printf(MENU_INPUT "Seleccione sector: " RESET);
    if (scanf("%d", &sectorIdx) != 1) {
        printf(MSG_ERROR "Entrada invalida\n" RESET);
        while(getchar() != '\n');
        return;
    }

    int idxSectorReal = sectorIdx - 1;

    if (idxSectorReal < 0 || idxSectorReal >= evento->sitio->totalSectores) {
        printf(MSG_ERROR "Sector invalido.\n" RESET);
        return;
    }

    printf(MSG_INFO "\nAsientos disponibles:\n" RESET);
    for (int i = 0; i < evento->sitio->sectores[idxSectorReal].cantidadEspacios; i++) {
        printf("%d. %c%d\n", i + 1,
               evento->sitio->sectores[idxSectorReal].inicial,
               i + 1);
    }

    printf(MENU_INPUT "Seleccione asiento: " RESET);
    if (scanf("%d", &asientoIdx) != 1) {
        printf(MSG_ERROR "Entrada invalida\n" RESET);
        while(getchar() != '\n');
        return;
    }

    if (!verificarDisponibilidad(evento, idxSectorReal, asientoIdx - 1)) {
        printf(MSG_ERROR "Asiento no disponible.\n" RESET);
        return;
    }

    float precio = obtenerPrecioAsiento(evento, idxSectorReal, asientoIdx - 1);
    printf(MSG_INFO "Precio: %.2f\n" RESET, precio);

    marcarVendido(evento, idxSectorReal, asientoIdx - 1);
}

void menuCliente() {
    char nombre[50];

    printf("\n" MENU_INPUT "Nombre del cliente: " RESET);
    scanf("%49s", nombre);

    printf(MSG_SUCCESS "Bienvenid@ %s\n" RESET, nombre);

    int opcion;

    do {
        printf("\n" MENU_BORDER "=================================\n" RESET);
        printf(MENU_TITLE "          MENU CLIENTE\n" RESET);
        printf(MENU_BORDER "=================================\n" RESET);

        printf(MENU_OPTION "1. Consultar eventos\n" RESET);
        printf(MENU_OPTION "2. Comprar boletos\n" RESET);
        printf(MENU_OPTION "3. Volver\n" RESET);

        printf(MENU_BORDER "=================================\n" RESET);

        printf(MENU_INPUT "Seleccione: " RESET);
        scanf("%d", &opcion);

        switch(opcion) {
            case 1:
                printf(MSG_INFO "Aqui consultas eventos...\n" RESET);
                break;

            case 2:
                printf(MENU_BORDER "=================================\n" RESET);
                printf(MSG_INFO "Aqui compras boletos...\n" RESET);
                comprarBoletos();
                break;

            case 3:
                printf(MSG_INFO "Volviendo...\n" RESET);
                break;

            default:
                printf(MSG_ERROR "Opcion invalida\n" RESET);
        }

    } while(opcion != 3);
}