// src/sitio.c
#include "../include/sitio.h"
#include "../include/sector.h"
#include "../include/asiento.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SitioEvento *sitios = NULL;
int totalSitios = 0;

// Inicializa la lista de sitios vacia
void inicializarSitios() {
    if (sitios != NULL) {
        // Si ya habia algo, se libera todo
        liberarSitios();
    }
    sitios = NULL;
    totalSitios = 0;
}

// Libera toda la memoria de los sitios incluyendo sectores y asientos
void liberarSitios() {
    for (int i = 0; i < totalSitios; i++) {
        resetSectoresDeSitio(&sitios[i]);   // libera sectores de cada sitio
    }
    free(sitios);
    sitios = NULL;
    totalSitios = 0;
}

// Agrega un sitio sin sectores inicialmente
void agregarSitio(const char *nombre, const char *ubicacion, const char *web) {
    // Verificar duplicado
    for (int i = 0; i < totalSitios; i++) {
        if (strcmp(sitios[i].nombre, nombre) == 0) {
            printf("Error: Ya existe un sitio con nombre '%s'.\n", nombre);
            return;
        }
    }

    // Ampliar arreglo de sitios
    SitioEvento *temp = realloc(sitios, (totalSitios + 1) * sizeof(SitioEvento));
    if (temp == NULL) {
        printf("Error de memoria al agregar sitio.\n");
        return;
    }
    sitios = temp;

    // Inicializar el nuevo sitio
    SitioEvento *nuevo = &sitios[totalSitios];
    strncpy(nuevo->nombre, nombre, MAX_NOMBRE - 1);
    nuevo->nombre[MAX_NOMBRE - 1] = '\0';
    strncpy(nuevo->ubicacion, ubicacion, MAX_UBICACION - 1);
    nuevo->ubicacion[MAX_UBICACION - 1] = '\0';
    strncpy(nuevo->web, web, MAX_WEB - 1);
    nuevo->web[MAX_WEB - 1] = '\0';
    nuevo->sectores = NULL;
    nuevo->totalSectores = 0;

    totalSitios++;
    printf("Sitio '%s' agregado correctamente.\n", nombre);
}

// --- Funciones para manejo de sectores ---

void agregarSectorASitio(SitioEvento *sitio, const char *nombreSector, char inicial, int cantidad) {
    if (cantidad <= 0) {
        printf("La cantidad de espacios debe ser mayor a 0.\n");
        return;
    }

    // Verificar que no exista un sector con el mismo nombre en este sitio
    for (int i = 0; i < sitio->totalSectores; i++) {
        if (strcmp(sitio->sectores[i].nombre, nombreSector) == 0) {
            printf("Error: Ya existe un sector con nombre '%s' en este sitio.\n", nombreSector);
            return;
        }
    }

    // Ampliar arreglo de sectores del sitio
    Sector *temp = realloc(sitio->sectores, (sitio->totalSectores + 1) * sizeof(Sector));
    if (temp == NULL) {
        printf("Error de memoria al agregar sector.\n");
        return;
    }
    sitio->sectores = temp;

    // Inicializar el nuevo sector
    Sector *nuevo = &sitio->sectores[sitio->totalSectores];
    strncpy(nuevo->nombre, nombreSector, MAX_SECTOR_NOMBRE - 1);
    nuevo->nombre[MAX_SECTOR_NOMBRE - 1] = '\0';
    nuevo->inicial = inicial;
    nuevo->cantidadEspacios = cantidad;

    // Crear arreglo de asientos
    nuevo->asientos = crearAsientos(cantidad, inicial);
    if (nuevo->asientos == NULL) {
        printf("Error de memoria para asientos.\n");
        return;
    }

    sitio->totalSectores++;
    printf("Sector '%s' agregado con %d asientos (ej. %c1...).\n", nombreSector, cantidad, inicial);
}

void resetSectoresDeSitio(SitioEvento *sitio) {
    // Liberar cada sector y sus asientos
    for (int i = 0; i < sitio->totalSectores; i++) {
        liberarAsientos(sitio->sectores[i].asientos, sitio->sectores[i].cantidadEspacios);
    }
    free(sitio->sectores);
    sitio->sectores = NULL;
    sitio->totalSectores = 0;
    printf("Espacios del sitio '%s' reiniciados.\n", sitio->nombre);
}


void mostrarSitios() {
    printf("\n=== Lista de sitios ===\n");
    for (int i = 0; i < totalSitios; i++) {
        printf("%d. %s - %s - %s\n", i+1, sitios[i].nombre, sitios[i].ubicacion, sitios[i].web);
    }
}