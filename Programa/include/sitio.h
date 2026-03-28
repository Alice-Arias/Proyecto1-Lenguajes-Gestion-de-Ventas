#ifndef SITIO_H
#define SITIO_H

#include "sector.h"

#define MAX_NOMBRE 50
#define MAX_UBICACION 150
#define MAX_WEB 100

/*
Estructura: SitioEvento

Descripcion:
Representa un sitio donde se realizan eventos.
Contiene información básica y sus sectores asociados.
*/
typedef struct
{
    char nombre[MAX_NOMBRE];
    char ubicacion[MAX_UBICACION];
    char web[MAX_WEB];
    Sector *sectores;
    int totalSectores;
} SitioEvento;

// Variables globales
extern SitioEvento *listaSitios;
extern int cantidadSitios;

// Inicialización y liberación
void inicializarSitios();
void liberarSitios();

// Gestión de sitios
void agregarSitio(const char *nombre, const char *ubicacion, const char *web);
void cargarSitiosDesdeArchivo(const char *ruta);
void guardarSitiosEnArchivo(const char *ruta);

// Gestión de sectores
void agregarSectorASitio(SitioEvento *sitio, const char *nombreSector, char inicial, int cantidad);
void resetSectoresDeSitio(SitioEvento *sitio);

// Visualización
void mostrarSitios();
void mostrarSectoresDeSitio(SitioEvento *sitio);

#endif