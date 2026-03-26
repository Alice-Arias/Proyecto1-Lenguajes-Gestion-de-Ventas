#include "../include/sector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


Sector crearSector(const char *nombre, char inicial, int cantidad) {

    Sector nuevo;

    strncpy(nuevo.nombre, nombre, MAX_SECTOR_NOMBRE - 1);
    nuevo.nombre[MAX_SECTOR_NOMBRE - 1] = '\0';

    nuevo.inicial = inicial;
    nuevo.cantidadEspacios = cantidad;

    nuevo.asientos = crearAsientos(cantidad, inicial);

    return nuevo;
}


void liberarSector(Sector *sector) {
    liberarAsientos(sector->asientos, sector->cantidadEspacios);
}


void mostrarSector(Sector *sector) {

    printf("\nSector: %s (%c)\n", sector->nombre, sector->inicial);
    printf("Cantidad: %d\n", sector->cantidadEspacios);

    printf("Asientos:\n");

    for (int i = 0; i < sector->cantidadEspacios; i++) {
        printf("  %c%d\n", sector->inicial, i + 1);
    }
}