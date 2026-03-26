#include <stdio.h>
#include "cliente.h"
#include "../include/colors.h"
#include "../include/evento.h"
#include "../include/sitio.h"

void comprarBoletos() {
    int eventoIdx, sectorIdx, asientoIdx;

    mostrarEventos();
    printf(MENU_INPUT "Seleccione evento: " RESET);
    scanf("%d", &eventoIdx);

    if (eventoIdx < 1 || eventoIdx > totalEventos) {
        printf(MSG_ERROR "Evento invalido.\n" RESET);
        return;
    }

    Evento *evento = &eventos[eventoIdx - 1];

    printf(MENU_INPUT "Seleccione sector: " RESET);
    scanf("%d", &sectorIdx);

    if (sectorIdx < 1 || sectorIdx > evento->sitio->totalSectores) {
        printf(MSG_ERROR "Sector invalido.\n" RESET);
        return;
    }

    printf(MENU_INPUT "Seleccione asiento: " RESET);
    scanf("%d", &asientoIdx);

    if (asientoIdx < 1 || asientoIdx > evento->sitio->sectores[sectorIdx - 1].cantidadEspacios) {
        printf(MSG_ERROR "Asiento invalido.\n" RESET);
        return;
    }

    if (!verificarDisponibilidad(evento, sectorIdx - 1, asientoIdx - 1)) {
        printf(MSG_ERROR "Asiento no disponible.\n" RESET);
        return;
    }

    float precio = obtenerPrecioAsiento(evento, sectorIdx - 1, asientoIdx - 1);
    printf(MSG_INFO "Precio: %.2f\n" RESET, precio);

    marcarVendido(evento, sectorIdx - 1, asientoIdx - 1);
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