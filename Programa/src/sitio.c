#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sitio.h"

SitioEvento *sitios = NULL;
int totalSitios = 0;

void inicializarSitios() {
    if (sitios != NULL) {
        free(sitios);
        sitios = NULL;
    }
    totalSitios = 0;
}

void agregarSitio(const char *nombre, const char *ubicacion, const char *web) {
    // Verificar si ya existe un sitio con el mismo nombre
    for (int i = 0; i < totalSitios; i++) {
        if (strcmp(sitios[i].nombre, nombre) == 0) {
            printf("Error: Ya existe un sitio con el nombre '%s'.\n", nombre);
            return;
        }
    }

    // Ampliar el arreglo dinámico
    SitioEvento *temp = realloc(sitios, (totalSitios + 1) * sizeof(SitioEvento));
    if (temp == NULL) {
        printf("Error de memoria al agregar sitio.\n");
        return;
    }
    sitios = temp;

    // Copiar los datos al nuevo elemento
    strncpy(sitios[totalSitios].nombre, nombre, MAX_NOMBRE - 1);
    sitios[totalSitios].nombre[MAX_NOMBRE - 1] = '\0';
    strncpy(sitios[totalSitios].ubicacion, ubicacion, MAX_UBICACION - 1);
    sitios[totalSitios].ubicacion[MAX_UBICACION - 1] = '\0';
    strncpy(sitios[totalSitios].web, web, MAX_WEB - 1);
    sitios[totalSitios].web[MAX_WEB - 1] = '\0';

    totalSitios++;
    printf("Sitio '%s' agregado correctamente.\n", nombre);
}

void cargarSitiosDesdeArchivo(const char *ruta) {
    FILE *f = fopen(ruta, "r");
    if (!f) {
        // Si no existe el archivo, no hay error, solo se inicializa vacio
        return;
    }

    char linea[512];
    while (fgets(linea, sizeof(linea), f)) {
        // Eliminar salto de línea
        linea[strcspn(linea, "\n")] = '\0';
        
        // Separar campos por coma - formato: nombre,ubicacion,web
        char *nombre = strtok(linea, ",");
        char *ubicacion = strtok(NULL, ",");
        char *web = strtok(NULL, ",");
        
        if (nombre && ubicacion && web) {
            agregarSitio(nombre, ubicacion, web);
        } else {
            printf("Formato inválido en línea: %s\n", linea);
        }
    }
    fclose(f);
}

void guardarSitiosEnArchivo(const char *ruta) {
    FILE *f = fopen(ruta, "w");
    if (!f) {
        printf("Error al abrir archivo %s para escritura.\n", ruta);
        return;
    }
    for (int i = 0; i < totalSitios; i++) {
        fprintf(f, "%s,%s,%s\n", sitios[i].nombre, sitios[i].ubicacion, sitios[i].web);
    }
    fclose(f);
}

void liberarSitios() {
    if (sitios) {
        free(sitios);
        sitios = NULL;
        totalSitios = 0;
    }
}

void mostrarSitios() {
    printf("=== Lista de sitios ===\n");
    for (int i = 0; i < totalSitios; i++) {
        printf("%d. %s - %s - %s\n", i+1, sitios[i].nombre, sitios[i].ubicacion, sitios[i].web);
    }
}