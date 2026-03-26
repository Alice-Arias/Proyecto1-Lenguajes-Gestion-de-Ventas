#ifndef SECTOR_H
#define SECTOR_H

#include "asiento.h"

#define MAX_SECTOR_NOMBRE 100

typedef struct {
    char nombre[MAX_SECTOR_NOMBRE];
    char inicial;
    int cantidadEspacios;
    Asiento *asientos;
} Sector;

Sector crearSector(const char *nombre, char inicial, int cantidad);
void liberarSector(Sector *sector);
void mostrarSector(Sector *sector);

#endif