#include "../include/asiento.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/colors.h"
#include "../include/evento.h"
#include "../include/sitio.h"
#include "../include/factura.h"

static void limpiarEspaciosInicio(char **texto) {
    while (**texto == ' ' || **texto == '\t') {
        (*texto)++;
    }
}

static Evento *buscarEventoPorNombre(const char *nombreEvento) {
    for (int i = 0; i < totalEventos; i++) {
        if (strcmp(eventos[i].nombre, nombreEvento) == 0) {
            return &eventos[i];
        }
    }
    return NULL;
}

static int buscarSectorPorInicial(const Evento *evento, char inicial) {
    char inicialMayus = (char)toupper((unsigned char)inicial);
    for (int s = 0; s < evento->sitio->totalSectores; s++) {
        char inicialSector = (char)toupper((unsigned char)evento->sitio->sectores[s].inicial);
        char inicialNombre = (char)toupper((unsigned char)evento->sitio->sectores[s].nombre[0]);
        if (inicialSector == inicialMayus || inicialNombre == inicialMayus) {
            return s;
        }
    }
    return -1;
}

static void aplicarDetalleFacturaAEvento(Evento *evento, const char *detalleAsientos) {
    if (evento == NULL || detalleAsientos == NULL || detalleAsientos[0] == '\0' || strcmp(detalleAsientos, "N/A") == 0) {
        return;
    }

    char detalleCopia[MAX_DETALLE_FACTURA];
    strncpy(detalleCopia, detalleAsientos, sizeof(detalleCopia) - 1);
    detalleCopia[sizeof(detalleCopia) - 1] = '\0';

    char *token = strtok(detalleCopia, ",");
    while (token != NULL) {
        limpiarEspaciosInicio(&token);

        if (token[0] != '\0') {
            char inicial = token[0];
            char *ptrNumero = token + 1;
            int numeroAsiento = (int)strtol(ptrNumero, NULL, 10);

            int sectorIdx = buscarSectorPorInicial(evento, inicial);
            if (sectorIdx >= 0 && numeroAsiento > 0) {
                int asientoIdx = numeroAsiento - 1;
                int totalAsientos = evento->sitio->sectores[sectorIdx].cantidadEspacios;
                if (asientoIdx < totalAsientos) {
                    evento->disponibilidad[sectorIdx][asientoIdx] = 0;
                }
            }
        }

        token = strtok(NULL, ",");
    }
}

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
        printf(MSG_ERROR "Error al abrir %s para guardar asientos.\n" RESET, ruta);
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
    printf(MSG_SUCCESS "Asientos guardados en %s.\n" RESET, ruta);
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
    printf(MSG_SUCCESS "Asientos cargados desde %s.\n" RESET, ruta);
}

void sincronizarAsientosConFacturas(const char *rutaFacturas) {
    Factura *facturas = NULL;
    int cantidad = 0;

    if (!facturaCargarTodas(rutaFacturas, &facturas, &cantidad)) {
        return;
    }

    for (int i = 0; i < cantidad; i++) {
        Evento *evento = buscarEventoPorNombre(facturas[i].evento);
        if (evento == NULL) {
            continue;
        }
        aplicarDetalleFacturaAEvento(evento, facturas[i].detalleAsientos);
    }

    facturaLiberarLista(facturas);
}