#include <stdio.h>
#include "cliente.h"

void menuCliente() {
    char nombre[50];

    printf("\nNombre del cliente: ");
    scanf("%49s", nombre);

    printf("Bienvenid@ %s\n", nombre);

    int opcion;

    do {
        printf("\n========== MENU CLIENTE ==========\n");
        printf("1. Consultar eventos\n");
        printf("2. Comprar boletos\n");
        printf("3. Volver\n");

        printf("Seleccione: ");
        scanf("%d", &opcion);

        switch(opcion) {
            case 1:
                printf("Aqui consultas eventos...\n");
                break;
            case 2:
                printf("Aqui compras boletos...\n");
                break;
            case 3:
                printf("Volviendo...\n");
                break;
            default:
                printf("Opcion invalida\n");
        }

    } while(opcion != 3);
}