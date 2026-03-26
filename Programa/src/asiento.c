#include "../include/asiento.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/colors.h"

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