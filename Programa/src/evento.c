#include "../include/evento.h"
#include "../include/sitio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Evento *eventos = NULL;
int totalEventos = 0;

void inicializarEventos() {
    if (eventos != NULL) {
        liberarEventos();
    }
    eventos = NULL;
    totalEventos = 0;
}

void crearEvento(const char *nombre, const char *productora, const char *fecha,
                 SitioEvento *sitio, float *preciosPorSector) {
    if (sitio == NULL) {
        printf("Error: Sitio no válido.\n");
        return;
    }

    // Verificar que no exista un evento con el mismo nombre
    for (int i = 0; i < totalEventos; i++) {
        if (strcmp(eventos[i].nombre, nombre) == 0) {
            printf("Error: Ya existe un evento con nombre '%s'.\n", nombre);
            return;
        }
    }

    // Ampliar arreglo de eventos
    Evento *temp = realloc(eventos, (totalEventos + 1) * sizeof(Evento));
    if (temp == NULL) {
        printf("Error de memoria al crear evento.\n");
        return;
    }
    eventos = temp;

    Evento *nuevo = &eventos[totalEventos];
    strncpy(nuevo->nombre, nombre, MAX_NOMBRE_EVENTO - 1);
    nuevo->nombre[MAX_NOMBRE_EVENTO - 1] = '\0';
    strncpy(nuevo->productora, productora, MAX_PRODUCTORA - 1);
    nuevo->productora[MAX_PRODUCTORA - 1] = '\0';
    strncpy(nuevo->fecha, fecha, MAX_FECHA - 1);
    nuevo->fecha[MAX_FECHA - 1] = '\0';
    nuevo->sitio = sitio;

    // Reservar memoria para los precios (uno por sector del sitio) ----
    int numSectores = sitio->totalSectores;
    nuevo->precios = (PrecioSector*) malloc(numSectores * sizeof(PrecioSector));
    if (nuevo->precios == NULL) {
        printf("Error de memoria para precios.\n");

        return;
    }
    for (int i = 0; i < numSectores; i++) {
        strncpy(nuevo->precios[i].nombreSector, sitio->sectores[i].nombre, MAX_SECTOR_NOMBRE - 1);
        nuevo->precios[i].nombreSector[MAX_SECTOR_NOMBRE - 1] = '\0';
        nuevo->precios[i].precio = preciosPorSector[i];
    }

    // Crear matriz de disponibilidad: para cada sector, un arreglo de bytes (1 = disponible)
    nuevo->disponibilidad = (int**) malloc(numSectores * sizeof(int*));
    if (nuevo->disponibilidad == NULL) {
        free(nuevo->precios);
        printf("Error de memoria para disponibilidad.\n");
        return;
    }
    for (int i = 0; i < numSectores; i++) {
        int asientosPorSector = sitio->sectores[i].cantidadEspacios;
        nuevo->disponibilidad[i] = (int*) calloc(asientosPorSector, sizeof(int));
        if (nuevo->disponibilidad[i] == NULL) {
            // Limpiar lo ya asignado
            for (int j = 0; j < i; j++) free(nuevo->disponibilidad[j]);
            free(nuevo->disponibilidad);
            free(nuevo->precios);
            printf("Error de memoria para disponibilidad sector %d.\n", i);
            return;
        }
        // Inicializar todos a 1 (disponible)
        for (int j = 0; j < asientosPorSector; j++) {
            nuevo->disponibilidad[i][j] = 1;
        }
    }

    totalEventos++;
    printf("Evento '%s' creado correctamente en sitio '%s'.\n", nombre, sitio->nombre);
}

void liberarEvento(Evento *evento) {
    if (evento == NULL) return;
    if (evento->precios) free(evento->precios);
    if (evento->disponibilidad) {
        for (int i = 0; i < evento->sitio->totalSectores; i++) {
            free(evento->disponibilidad[i]);
        }
        free(evento->disponibilidad);
    }
}

void liberarEventos() {
    for (int i = 0; i < totalEventos; i++) {
        liberarEvento(&eventos[i]);
    }
    free(eventos);
    eventos = NULL;
    totalEventos = 0;
}

void guardarEventosEnArchivo(const char *ruta) {
    FILE *f = fopen(ruta, "w");
    if (!f) {
        printf("Error al abrir archivo %s para guardar eventos.\n", ruta);
        return;
    }

    for (int i = 0; i < totalEventos; i++) {
        Evento *e = &eventos[i];
        // Formato: nombre,productora,fecha,sitio_nombre,precio1,precio2,...
        fprintf(f, "%s,%s,%s,%s", e->nombre, e->productora, e->fecha, e->sitio->nombre);
        for (int s = 0; s < e->sitio->totalSectores; s++) {
            fprintf(f, ",%.2f", e->precios[s].precio);
        }
        fprintf(f, "\n");

    }
    fclose(f);
    printf("Eventos guardados en %s.\n", ruta);
}

void cargarEventosDesdeArchivo(const char *ruta) {
    FILE *f = fopen(ruta, "r");
    if (!f) {
        // No existe archivo, no hay error
        return;
    }

    // Leer línea por línea
    char linea[1024];
    while (fgets(linea, sizeof(linea), f)) {
        linea[strcspn(linea, "\n")] = '\0';
        char *token = strtok(linea, ",");
        if (!token) continue;
        char nombre[MAX_NOMBRE_EVENTO];
        strncpy(nombre, token, MAX_NOMBRE_EVENTO - 1);
        nombre[MAX_NOMBRE_EVENTO - 1] = '\0';

        char productora[MAX_PRODUCTORA];
        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(productora, token, MAX_PRODUCTORA - 1);
        productora[MAX_PRODUCTORA - 1] = '\0';

        char fecha[MAX_FECHA];
        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(fecha, token, MAX_FECHA - 1);
        fecha[MAX_FECHA - 1] = '\0';

        char nombreSitio[MAX_NOMBRE];
        token = strtok(NULL, ",");
        if (!token) continue;
        strncpy(nombreSitio, token, MAX_NOMBRE - 1);
        nombreSitio[MAX_NOMBRE - 1] = '\0';

        // Buscar el sitio por nombre
        SitioEvento *sitio = NULL;
for (int i = 0; i < cantidadSitios; i++) {
    if (strcmp(listaSitios[i].nombre, nombreSitio) == 0) {
                sitio = &listaSitios[i];
                break;
            }
        }
        if (sitio == NULL) {
            printf("Advertencia: sitio '%s' no encontrado para evento '%s'\n", nombreSitio, nombre);
            continue;
        }

        int numSectores = sitio->totalSectores;
        float *precios = (float*) malloc(numSectores * sizeof(float));
        if (precios == NULL) {
            printf("Error de memoria al cargar precios.\n");
            fclose(f);
            return;
        }
        for (int i = 0; i < numSectores; i++) {
            token = strtok(NULL, ",");
            if (token) {
                precios[i] = atof(token);
            } else {
                precios[i] = 0.0;
            }
        }
        crearEvento(nombre, productora, fecha, sitio, precios);
        free(precios);
    }
    fclose(f);
}

void mostrarEventos() {
    printf("\n=== Lista de eventos ===\n");
    for (int i = 0; i < totalEventos; i++) {
        Evento *e = &eventos[i];
        printf("%d. %s - %s - %s - Sitio: %s\n", i+1, e->nombre, e->productora, e->fecha, e->sitio->nombre);
    }
}

int verificarDisponibilidad(Evento *evento, int sectorIdx, int asientoIdx) {
    if (evento == NULL || sectorIdx < 0 || sectorIdx >= evento->sitio->totalSectores) return 0;
    if (asientoIdx < 0 || asientoIdx >= evento->sitio->sectores[sectorIdx].cantidadEspacios) return 0;
    return evento->disponibilidad[sectorIdx][asientoIdx];
}

void marcarVendido(Evento *evento, int sectorIdx, int asientoIdx) {
    if (evento == NULL) return;
    if (sectorIdx >= 0 && sectorIdx < evento->sitio->totalSectores &&
        asientoIdx >= 0 && asientoIdx < evento->sitio->sectores[sectorIdx].cantidadEspacios) {
        evento->disponibilidad[sectorIdx][asientoIdx] = 0;
    }
}

float obtenerPrecioAsiento(Evento *evento, int sectorIdx, int asientoIdx) {
    if (evento == NULL || sectorIdx < 0 || sectorIdx >= evento->sitio->totalSectores) return 0.0;
    return evento->precios[sectorIdx].precio;
}