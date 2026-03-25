#ifndef SITIO_H
#define SITIO_H

#include "sector.h"

#define MAX_NOMBRE 100
#define MAX_UBICACION 150
#define MAX_WEB 100

// Estructura para un sitio de eventos
typedef struct {
    char nombre[MAX_NOMBRE];
    char ubicacion[MAX_UBICACION];
    char web[MAX_WEB];
    Sector *sectores;
    int totalSectores;
} SitioEvento;

// se declaran extern para que sean accesibles desde otros documentos .c
extern SitioEvento *sitios;   
extern int totalSitios;       


void inicializarSitios();
void agregarSitio(const char *nombre, const char *ubicacion, const char *web);
void agregarSectorASitio(SitioEvento *sitio, const char *nombreSector, char inicial, int cantidad);
void resetSectoresDeSitio(SitioEvento *sitio);
void cargarSitiosDesdeArchivo(const char *ruta);
void guardarSitiosEnArchivo(const char *ruta);
void liberarSitios();
void mostrarSitios();

#endif
