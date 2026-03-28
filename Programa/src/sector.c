#include "../include/sector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/colors.h"
#include "../include/sitio.h"

/*
 * Objetivo: Construir un sector con su metadata y lista de asientos asociada.
 * Entradas: nombre del sector, inicial del asiento y cantidad de espacios.
 * Salidas: Estructura Sector inicializada; puede contener asientos en NULL si falla.
 * Restricciones: cantidad debe ser mayor a 0 para crear asientos validos.
 */
Sector crearSector(const char *nombre, char inicial, int cantidad) {

    Sector nuevo;

    if (cantidad <= 0) {
        printf(MSG_ERROR "Cantidad invalida para el sector.\n" RESET);
        nuevo.asientos = NULL;
        nuevo.cantidadEspacios = 0;
        return nuevo;
    }

    strncpy(nuevo.nombre, nombre, MAX_SECTOR_NOMBRE - 1);
    nuevo.nombre[MAX_SECTOR_NOMBRE - 1] = '\0';

    nuevo.inicial = inicial;
    nuevo.cantidadEspacios = cantidad;

    // Crear asientos
    nuevo.asientos = crearAsientos(cantidad, inicial);

    if (nuevo.asientos == NULL) {
        printf(MSG_ERROR "Error al crear asientos del sector.\n" RESET);
    }

    return nuevo;
}

/*
 * Objetivo: Persistir todos los sectores de todos los sitios en archivo de texto.
 * Entradas: ruta del archivo destino.
 * Salidas: Escribe los registros en disco y muestra mensaje de estado.
 * Restricciones: la ruta debe ser accesible en modo escritura.
 */
void guardarSectoresEnArchivo(const char *ruta) {
    FILE *f = fopen(ruta, "w");
    if (!f) {
        printf(MSG_ERROR "Error al guardar sectores.\n" RESET);
        return;
    }

    for (int i = 0; i < cantidadSitios; i++) {
        SitioEvento *sitio = &listaSitios[i];

        for (int j = 0; j < sitio->totalSectores; j++) {
            Sector *s = &sitio->sectores[j];

            fprintf(f, "%s,%s,%c,%d\n",
                sitio->nombre,
                s->nombre,
                s->inicial,
                s->cantidadEspacios
            );
        }
    }

    fclose(f);
    printf(MSG_SUCCESS "Sectores guardados correctamente.\n" RESET);
}

/*
 * Objetivo: Cargar sectores desde archivo y asociarlos al sitio correspondiente.
 * Entradas: ruta del archivo con formato sitio,sector,inicial,cantidad.
 * Salidas: Agrega sectores a los sitios encontrados en memoria.
 * Restricciones: los sitios deben existir previamente para poder asignar sectores.
 */
void cargarSectoresDesdeArchivo(const char *ruta) {
    FILE *f = fopen(ruta, "r");
    if (!f) return;

    char linea[200];

    while (fgets(linea, sizeof(linea), f)) {
        linea[strcspn(linea, "\n")] = '\0';

        char nombreSitio[50], nombreSector[50];
        char inicial;
        int cantidad;

        sscanf(linea, "%[^,],%[^,],%c,%d",
            nombreSitio,
            nombreSector,
            &inicial,
            &cantidad
        );

        for (int i = 0; i < cantidadSitios; i++) {
            if (strcmp(listaSitios[i].nombre, nombreSitio) == 0) {

                agregarSectorASitio(
                    &listaSitios[i],
                    nombreSector,
                    inicial,
                    cantidad
                );

                break;
            }
        }
    }

    fclose(f);
}

/*
 * Objetivo: Liberar memoria dinamica utilizada por los asientos de un sector.
 * Entradas: puntero al sector a liberar.
 * Salidas: Libera recursos del arreglo de asientos.
 * Restricciones: sector debe apuntar a una estructura valida.
 */
void liberarSector(Sector *sector) {
    liberarAsientos(sector->asientos, sector->cantidadEspacios);
}


/*
 * Objetivo: Mostrar en consola la informacion y asientos de un sector.
 * Entradas: puntero al sector que se desea imprimir.
 * Salidas: Despliega datos formateados del sector en pantalla.
 * Restricciones: sector debe ser valido y contener datos coherentes.
 */
void mostrarSector(Sector *sector) {

    printf("\n" MENU_BORDER "============================\n" RESET);
    printf(COLOR_ASIENTO BOLD "        SECTOR\n" RESET);
    printf(MENU_BORDER "============================\n" RESET);

    printf(COLOR_SITIO "Nombre: %s" RESET, sector->nombre);
    printf(MENU_BORDER " | Inicial: " RESET);
    printf(HIGHLIGHT "%c\n" RESET, sector->inicial);

    printf(MSG_INFO "Cantidad: %d\n" RESET, sector->cantidadEspacios);

    printf(COLOR_ASIENTO "Asientos:\n" RESET);

    for (int i = 0; i < sector->cantidadEspacios; i++) {
        printf(COLOR_ASIENTO "  %c%d  " RESET, sector->inicial, i + 1);

        if ((i + 1) % 10 == 0)
            printf("\n");
    }

    printf("\n\n");
}