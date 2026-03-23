#include <stdio.h>
#include "admin/admin.h"
#include "cliente/cliente.h"
#include "config/sistema.h"

int main() {

    inicializarSistema();

    int opcion;

    do {
        printf("\n========= SISTEMA DE BOLETOS ==========\n");
        printf("1. Administrador\n");
        printf("2. Cliente\n");
        printf("3. Salir\n");
        printf("==========================================\n");

        printf("Seleccione: ");
        scanf("%d", &opcion);

        switch(opcion) {

            case 1:
                if (IniciarSesionAdmin()) {
                    printf("Acceso concedido.\n");
                    menuAdmin();
                } else {
                    printf("Acceso denegado.\n");
                }
                break;

            case 2:
                menuCliente();
                break;

            case 3:
                printf("Saliendo...\n");
                break;

            default:
                printf("Opcion invalida\n");
        }

    } while(opcion != 3);

    return 0;
}