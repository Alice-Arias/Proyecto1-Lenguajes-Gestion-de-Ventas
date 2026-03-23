// include/sector.h
#ifndef SECTOR_H
#define SECTOR_H

#define MAX_SECTOR_NOMBRE 100
#define MAX_ASIENTO_CODE 10


typedef struct {
    char codigo[MAX_ASIENTO_CODE];
} Asiento;

// Estructura para un sector
typedef struct {
    char nombre[MAX_SECTOR_NOMBRE];
    char inicial;               // letra que identifica al sector (ej. 'A')
    int cantidadEspacios;       
    Asiento *asientos;          
} Sector;

#endif