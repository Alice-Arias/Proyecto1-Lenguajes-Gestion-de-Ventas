#include "../include/sector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/colors.h"

Sector crearSector(const char *nombre, char inicial, int cantidad) {

    Sector nuevo;

    if (cantidad <= 0) {
        printf(MSG_ERROR "Cantidad invalida para el sector.\n" RESET);
        nuevo.asientos = NULL;
        nuevo.cantidadEspacios = 0;
        return nuevo;
    }

    strncpy(nuevo.nombre, nombre, MAX_SECTOR_NOMBRE - 1);
    nuevo.nombre[MAX_SECTOR_NOMBRE - 1] = '\0';

    nuevo.inicial = inicial;
    nuevo.cantidadEspacios = cantidad;

    // Crear asientos
    nuevo.asientos = crearAsientos(cantidad, inicial);

    if (nuevo.asientos == NULL) {
        printf(MSG_ERROR "Error al crear asientos del sector.\n" RESET);
    }

    return nuevo;
}

void liberarSector(Sector *sector) {
    liberarAsientos(sector->asientos, sector->cantidadEspacios);
}


void mostrarSector(Sector *sector) {

    printf("\n" MENU_BORDER "============================\n" RESET);
    printf(COLOR_ASIENTO BOLD "        SECTOR\n" RESET);
    printf(MENU_BORDER "============================\n" RESET);

    printf(COLOR_SITIO "Nombre: %s" RESET, sector->nombre);
    printf(" | Inicial: ");
    printf(HIGHLIGHT "%c\n" RESET, sector->inicial);

    printf(MSG_INFO "Cantidad: %d\n" RESET, sector->cantidadEspacios);

    printf(COLOR_ASIENTO "Asientos:\n" RESET);

    for (int i = 0; i < sector->cantidadEspacios; i++) {
        printf(COLOR_ASIENTO "  %c%d  " RESET, sector->inicial, i + 1);

        // Orden visual (cada 10)
        if ((i + 1) % 10 == 0)
            printf("\n");
    }

    printf("\n\n");
}