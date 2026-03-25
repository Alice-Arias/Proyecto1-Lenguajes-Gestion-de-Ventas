#include "../include/asiento.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Asiento* crearAsientos(int cantidad, char inicial) {
    if (cantidad <= 0) {
        return NULL;
    }

    Asiento *asientos = (Asiento*) malloc(cantidad * sizeof(Asiento));
    if (asientos == NULL) {
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
        printf("No hay asientos para mostrar.\n");
        return;
    }
    printf("Asientos:\n");
    for (int i = 0; i < cantidad; i++) {
        printf("  %s\n", asientos[i].codigo);
        if ((i+1) % 10 == 0) printf("\n");
    }
    printf("\n");
}