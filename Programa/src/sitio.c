#include "../include/sitio.h"
#include "../include/sector.h"
#include "../include/asiento.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


SitioEvento *listaSitios = NULL;
int cantidadSitios = 0;



void inicializarSitios() {
    if (listaSitios != NULL) {
        liberarSitios();
    }
    listaSitios = NULL;
    cantidadSitios = 0;
}


void liberarSitios() {
    for (int i = 0; i < cantidadSitios; i++) {
        resetSectoresDeSitio(&listaSitios[i]);
    }
    free(listaSitios);
    listaSitios = NULL;
    cantidadSitios = 0;
}


void agregarSitio(const char *nombre, const char *ubicacion, const char *web) {

    // Validar duplicados
    for (int i = 0; i < cantidadSitios; i++) {
        if (strcmp(listaSitios[i].nombre, nombre) == 0) {
            printf("Error: Ya existe un sitio con nombre '%s'.\n", nombre);
            return;
        }
    }

    // Redimensionar arreglo
    SitioEvento *nuevoBloque = realloc(listaSitios, (cantidadSitios + 1) * sizeof(SitioEvento));
    if (nuevoBloque == NULL) {
        printf("Error de memoria al agregar sitio.\n");
        return;
    }
    listaSitios = nuevoBloque;

    // Inicializar nuevo sitio
    SitioEvento *nuevoSitio = &listaSitios[cantidadSitios];

    strncpy(nuevoSitio->nombre, nombre, MAX_NOMBRE - 1);
    nuevoSitio->nombre[MAX_NOMBRE - 1] = '\0';

    strncpy(nuevoSitio->ubicacion, ubicacion, MAX_UBICACION - 1);
    nuevoSitio->ubicacion[MAX_UBICACION - 1] = '\0';

    strncpy(nuevoSitio->web, web, MAX_WEB - 1);
    nuevoSitio->web[MAX_WEB - 1] = '\0';

    nuevoSitio->sectores = NULL;
    nuevoSitio->totalSectores = 0;

    cantidadSitios++;

    printf("Sitio '%s' agregado correctamente.\n", nombre);
}


void agregarSectorASitio(SitioEvento *sitio, const char *nombreSector, char inicial, int cantidad) {

    if (cantidad <= 0) {
        printf("La cantidad de espacios debe ser mayor a 0.\n");
        return;
    }

    // Validar duplicados
    for (int i = 0; i < sitio->totalSectores; i++) {
        if (strcmp(sitio->sectores[i].nombre, nombreSector) == 0) {
            printf("Error: Sector '%s' ya existe.\n", nombreSector);
            return;
        }
    }

    // Redimensionar sectores
    Sector *nuevoBloque = realloc(sitio->sectores, (sitio->totalSectores + 1) * sizeof(Sector));
    if (nuevoBloque == NULL) {
        printf("Error de memoria al agregar sector.\n");
        return;
    }

    sitio->sectores = nuevoBloque;


    sitio->sectores[sitio->totalSectores] =
        crearSector(nombreSector, inicial, cantidad);

    sitio->totalSectores++;

    printf("Sector '%s' agregado correctamente.\n", nombreSector);
}

void resetSectoresDeSitio(SitioEvento *sitio) {

    for (int i = 0; i < sitio->totalSectores; i++) {
        liberarSector(&sitio->sectores[i]);
    }

    free(sitio->sectores);
    sitio->sectores = NULL;
    sitio->totalSectores = 0;

    printf("Sectores del sitio '%s' reiniciados.\n", sitio->nombre);
}


void mostrarSitios() {

    printf("\n=== Lista de sitios ===\n");

    for (int i = 0; i < cantidadSitios; i++) {
        printf("%d. %s - %s - %s\n",
            i + 1,
            listaSitios[i].nombre,
            listaSitios[i].ubicacion,
            listaSitios[i].web
        );
    }
}


void mostrarSectoresDeSitio(SitioEvento *sitio) {

    printf("\n=== Sectores del sitio '%s' ===\n", sitio->nombre);

    for (int i = 0; i < sitio->totalSectores; i++) {
        mostrarSector(&sitio->sectores[i]); 
    }
}

void cargarSitiosDesdeArchivo(const char *ruta) {

    FILE *archivo = fopen(ruta, "r");

    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return;
    }

    char linea[300];

    while (fgets(linea, sizeof(linea), archivo)) {

        linea[strcspn(linea, "\n")] = '\0';

        char nombre[MAX_NOMBRE];
        char ubicacion[MAX_UBICACION];
        char web[MAX_WEB] = "";

        char *token = strtok(linea, ",");

        if (token == NULL) continue;
        strcpy(nombre, token);

        token = strtok(NULL, ",");
        if (token == NULL) continue;
        strcpy(ubicacion, token);

        token = strtok(NULL, ",");
        if (token != NULL) {
            strcpy(web, token);
        }

        agregarSitio(nombre, ubicacion, web);
    }

    fclose(archivo);
}


void guardarSitiosEnArchivo(const char *ruta) {

    FILE *archivo = fopen(ruta, "w");

    if (archivo == NULL) {
        printf("Error al guardar archivo.\n");
        return;
    }

    for (int i = 0; i < cantidadSitios; i++) {
        fprintf(archivo, "%s,%s,%s\n",
            listaSitios[i].nombre,
            listaSitios[i].ubicacion,
            listaSitios[i].web
        );
    }

    fclose(archivo);

    printf("Sitios guardados correctamente.\n");
}