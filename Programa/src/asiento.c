#include "../include/asiento.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/colors.h"
#include "../include/evento.h"
#include "../include/sitio.h"

Asiento* crearAsientos(int cantidad, char inicial) {
        if (cantidad <= 0) {
            printf(MSG_ERROR "Cantidad invalida de asientos.\n" RESET);
                return NULL;
        }

    Asiento *asientos = (Asiento*) malloc(cantidad * sizeof(Asiento));
        if (asientos == NULL) {
            printf(MSG_ERROR "Error al asignar memoria para asientos.\n" RESET);
            return NULL;
        }

    // Calcular el número máximo de dígitos para el número más alto
    int max_num = cantidad;
    int digits = 0;
    if (max_num == 0) digits = 1;
    else {
        while (max_num > 0) {
            digits++;
            max_num /= 10;
        }
    }
    int length = 1 + digits + 1;  // char + digits + \0

    for (int i = 0; i < cantidad; i++) {
        asientos[i].codigo = (char*) malloc(length * sizeof(char));
        if (asientos[i].codigo == NULL) {
            printf(MSG_ERROR "Error al asignar memoria para codigo.\n" RESET);
            // Liberar memoria ya asignada
            for (int j = 0; j < i; j++) {
                free(asientos[j].codigo);
            }
            free(asientos);
            return NULL;
        }
        // Generar código: inicial + número (1-indexado)
        sprintf(asientos[i].codigo, "%c%d", inicial, i + 1);
    }

    return asientos;
}

void liberarAsientos(Asiento *asientos, int cantidad) {
    if (asientos != NULL) {
        for (int i = 0; i < cantidad; i++) {
            if (asientos[i].codigo != NULL) {
                free(asientos[i].codigo);
            }
        }
        free(asientos);
    }
}

void mostrarAsientos(const Asiento *asientos, int cantidad) {
    if (asientos == NULL || cantidad <= 0) {
        printf(MSG_WARNING "No hay asientos para mostrar.\n" RESET);
        return;
    }

    printf("\n" MENU_BORDER "=========================\n" RESET);
    printf(COLOR_ASIENTO BOLD "        ASIENTOS\n" RESET);
    printf(MENU_BORDER "=========================\n" RESET);

    for (int i = 0; i < cantidad; i++) {
        printf(COLOR_ASIENTO "  %s  " RESET, asientos[i].codigo);

        // Salto de línea cada 10 para que se vea ordenado
        if ((i + 1) % 10 == 0)
            printf("\n");
    }

    printf("\n\n");
}


void guardarAsientosEnArchivo(const char *ruta) {
    FILE *f = fopen(ruta, "w");
    if (!f) {
        printf("Error al abrir %s para guardar asientos.\n", ruta);
        return;
    }

    for (int i = 0; i < totalEventos; i++) {
        Evento *e = &eventos[i];
        fprintf(f, "%s", e->nombre);
        for (int s = 0; s < e->sitio->totalSectores; s++) {
            fprintf(f, ",%s,", e->sitio->sectores[s].nombre);
            for (int a = 0; a < e->sitio->sectores[s].cantidadEspacios; a++) {
                fprintf(f, "%d", e->disponibilidad[s][a]);
            }
        }
        fprintf(f, "\n");
    }

    fclose(f);
    printf("Asientos guardados en %s.\n", ruta);
}

    
void cargarAsientosDesdeArchivo(const char *ruta) {
    FILE *f = fopen(ruta, "r");
    if (!f) return;

    char linea[2048];
    while (fgets(linea, sizeof(linea), f)) {
        linea[strcspn(linea, "\n")] = '\0';
        char *token = strtok(linea, ",");
        if (!token) continue;

        char nombreEvento[MAX_NOMBRE_EVENTO];
        strncpy(nombreEvento, token, MAX_NOMBRE_EVENTO-1);
        nombreEvento[MAX_NOMBRE_EVENTO-1] = '\0';

        // Buscar evento
        Evento *evento = NULL;
        for (int i = 0; i < totalEventos; i++) {
            if (strcmp(eventos[i].nombre, nombreEvento) == 0) {
                evento = &eventos[i];
                break;
            }
        }
        if (!evento) continue;

        int s = 0;
        while ((token = strtok(NULL, ",")) && s < evento->sitio->totalSectores) {
            char *nombreSector = token;
            token = strtok(NULL, ",");
            if (!token) break;
            char *asientosStr = token;

            int nAsientos = evento->sitio->sectores[s].cantidadEspacios;
            for (int a = 0; a < nAsientos && asientosStr[a]; a++) {
                evento->disponibilidad[s][a] = asientosStr[a] - '0';
            }

            s++;
        }
    }

    fclose(f);
    printf("Asientos cargados desde %s.\n", ruta);
}