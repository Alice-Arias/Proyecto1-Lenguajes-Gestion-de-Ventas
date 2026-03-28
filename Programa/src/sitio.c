#include "../include/sitio.h"
#include "../include/sector.h"
#include "../include/asiento.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/colors.h"

SitioEvento *listaSitios = NULL;
int cantidadSitios = 0;


/*
 * Objetivo: Inicializar el contenedor dinamico de sitios en estado vacio.
 * Entradas: No recibe parametros.
 * Salidas: Reinicia listaSitios y cantidadSitios.
 * Restricciones: si ya existen datos en memoria, se liberan antes de reinicializar.
 */
void inicializarSitios() {
    if (listaSitios != NULL) {
        liberarSitios();
    }
    listaSitios = NULL;
    cantidadSitios = 0;
}


/*
 * Objetivo: Liberar todos los sitios y sus sectores asociados.
 * Entradas: No recibe parametros.
 * Salidas: Memoria de sitios completamente liberada.
 * Restricciones: debe llamarse antes de finalizar para evitar fugas de memoria.
 */
void liberarSitios() {
    for (int i = 0; i < cantidadSitios; i++) {
        resetSectoresDeSitio(&listaSitios[i]);
    }
    free(listaSitios);
    listaSitios = NULL;
    cantidadSitios = 0;
}


/*
 * Objetivo: Agregar un nuevo sitio al arreglo dinamico evitando duplicados por nombre.
 * Entradas: nombre, ubicacion y web del sitio.
 * Salidas: Inserta un sitio nuevo y actualiza cantidadSitios.
 * Restricciones: nombre debe ser unico dentro de la lista actual.
 */
void agregarSitio(const char *nombre, const char *ubicacion, const char *web) {

    // Validar duplicados
    for (int i = 0; i < cantidadSitios; i++) {
        if (strcmp(listaSitios[i].nombre, nombre) == 0) {
            printf(MSG_ERROR "Error: Ya existe un sitio con nombre '%s'.\n" RESET, nombre);
            return;
        }
    }

    // Redimensionar arreglo
    SitioEvento *nuevoBloque = realloc(listaSitios, (cantidadSitios + 1) * sizeof(SitioEvento));
    if (nuevoBloque == NULL) {
        printf(MSG_ERROR "Error de memoria al agregar sitio.\n" RESET);
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

    printf(MSG_SUCCESS "Sitio '%s' agregado correctamente.\n" RESET, nombre);
}


/*
 * Objetivo: Agregar un sector a un sitio existente con sus asientos inicializados.
 * Entradas: sitio destino, nombre del sector, inicial y cantidad de espacios.
 * Salidas: Amplia el arreglo de sectores del sitio.
 * Restricciones: cantidad > 0 y nombreSector no repetido dentro del sitio.
 */
void agregarSectorASitio(SitioEvento *sitio, const char *nombreSector, char inicial, int cantidad) {

    if (cantidad <= 0) {
        printf(MSG_ERROR "La cantidad de espacios debe ser mayor a 0.\n" RESET);
        return;
    }

    // Validar duplicados
    for (int i = 0; i < sitio->totalSectores; i++) {
        if (strcmp(sitio->sectores[i].nombre, nombreSector) == 0) {
            printf(MSG_ERROR "Error: Sector '%s' ya existe.\n" RESET, nombreSector);
            return;
        }
    }

    // Redimensionar sectores
    Sector *nuevoBloque = realloc(sitio->sectores, (sitio->totalSectores + 1) * sizeof(Sector));
    if (nuevoBloque == NULL) {
        printf(MSG_ERROR "Error de memoria al agregar sector.\n" RESET);
        return;
    }

    sitio->sectores = nuevoBloque;


    sitio->sectores[sitio->totalSectores] =
        crearSector(nombreSector, inicial, cantidad);

    sitio->totalSectores++;

    printf(MSG_SUCCESS "Sector '%s' agregado correctamente.\n" RESET, nombreSector);
}

/*
 * Objetivo: Eliminar todos los sectores de un sitio y dejarlo sin espacios.
 * Entradas: puntero al sitio a reiniciar.
 * Salidas: libera memoria de sectores y pone totalSectores en 0.
 * Restricciones: sitio debe apuntar a una estructura valida.
 */
void resetSectoresDeSitio(SitioEvento *sitio) {

    for (int i = 0; i < sitio->totalSectores; i++) {
        liberarSector(&sitio->sectores[i]);
    }

    free(sitio->sectores);
    sitio->sectores = NULL;
    sitio->totalSectores = 0;

    printf(MSG_SUCCESS "Sectores del sitio '%s' reiniciados.\n" RESET, sitio->nombre);
}


/*
 * Objetivo: Mostrar el listado actual de sitios registrados en memoria.
 * Entradas: No recibe parametros.
 * Salidas: imprime nombre, ubicacion y web de cada sitio.
 * Restricciones: usa el estado global listaSitios/cantidadSitios.
 */
void mostrarSitios() {

    printf("\n" MENU_BORDER "====================================\n" RESET);
    printf(COLOR_SITIO BOLD "         LISTA DE SITIOS\n" RESET);
    printf(MENU_BORDER "====================================\n" RESET);

    for (int i = 0; i < cantidadSitios; i++) {
        printf(COLOR_SITIO "%d. %s" RESET, i + 1, listaSitios[i].nombre);
        printf(MENU_BORDER " | " RESET);
        printf(MSG_INFO "%s" RESET, listaSitios[i].ubicacion);
        printf(MENU_BORDER " | " RESET);
        printf(COLOR_EVENTO "%s\n" RESET, listaSitios[i].web);
    }

    printf("\n");
}


/*
 * Objetivo: Mostrar todos los sectores pertenecientes a un sitio especifico.
 * Entradas: puntero al sitio del cual se desea listar sectores.
 * Salidas: imprime cada sector usando mostrarSector().
 * Restricciones: sitio debe ser valido y estar inicializado.
 */
void mostrarSectoresDeSitio(SitioEvento *sitio) {

    printf("\n" MENU_BORDER "====================================\n" RESET);
    printf(COLOR_SITIO BOLD " Sectores del sitio: %s\n" RESET, sitio->nombre);
    printf(MENU_BORDER "====================================\n" RESET);

    for (int i = 0; i < sitio->totalSectores; i++) {
        mostrarSector(&sitio->sectores[i]);
    }
}

/*
 * Objetivo: Cargar sitios desde un archivo de texto y agregarlos en memoria.
 * Entradas: ruta del archivo fuente.
 * Salidas: inserta sitios validos al arreglo dinamico.
 * Restricciones: cada linea debe contener nombre,ubicacion[,web].
 */
void cargarSitiosDesdeArchivo(const char *ruta) {

    FILE *archivo = fopen(ruta, "r");

    if (archivo == NULL) {
        printf(MSG_ERROR "No se pudo abrir el archivo.\n" RESET);
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


/*
 * Objetivo: Guardar todos los sitios actuales en archivo de texto plano.
 * Entradas: ruta del archivo destino.
 * Salidas: escribe una linea por sitio con nombre, ubicacion y web.
 * Restricciones: requiere permisos de escritura sobre la ruta indicada.
 */
void guardarSitiosEnArchivo(const char *ruta) {

    FILE *archivo = fopen(ruta, "w");

    if (archivo == NULL) {
        printf(MSG_ERROR "Error al guardar archivo.\n" RESET);
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

    printf(MSG_SUCCESS "Sitios guardados correctamente.\n" RESET);
}