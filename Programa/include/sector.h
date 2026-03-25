// include/sector.h
#ifndef SECTOR_H
#define SECTOR_H

#include "asiento.h" 

#define MAX_SECTOR_NOMBRE 100

// Estructura para un sector
typedef struct {
    char nombre[MAX_SECTOR_NOMBRE];
    char inicial;               
    int cantidadEspacios;       
    Asiento *asientos;          
} Sector;

#endif