#include <stdio.h>
#include "admin/admin.h"
#include "cliente/cliente.h"
#include "include/sitio.h"
#include "include/evento.h"
#include "include/sector.h"
#include "include/asiento.h"
#include "include/colors.h"

void probarEstructuras() {
    printf(COLOR_BLUE "\n=== Probando estructuras ===\n" COLOR_RESET);

    // Crear sitio
    agregarSitio("Estadio Nacional", "Santiago, Chile", "www.estadio.cl");

    // Agregar sector al sitio
    agregarSectorASitio(&sitios[0], "Tribuna Norte", 'A', 10);

    // Crear evento
    float precios[] = {50.0}; // precio para el sector
    crearEvento("Concierto Rock", "Productora X", "01/12/2023", &sitios[0], precios);

    // Mostrar
    mostrarSitios();
    mostrarSectoresDeSitio(&sitios[0]);
    mostrarAsientos(sitios[0].sectores[0].asientos, 10);
    mostrarEventos();

    printf(COLOR_BLUE "\n=== Fin de prueba ===\n" COLOR_RESET);
}

int main() {
    cargarUsuarios();
    inicializarSitios();
    inicializarEventos();

    int opcion;

    do {
        // Título con color cian y negrita
        printf(MENU_TITLE "\n========= SISTEMA DE BOLETOS =========\n" COLOR_RESET);
        // Opciones en verde
        printf(MENU_OPTION "1. Administrador\n" COLOR_RESET);
        printf(MENU_OPTION "2. Cliente\n" COLOR_RESET);
        printf(MENU_OPTION "4. Probar Estructuras\n" COLOR_RESET);
        printf(MENU_OPTION "5. Salir\n" COLOR_RESET);
        printf("======================================\n");

        printf("Seleccione: ");
        scanf("%d", &opcion);

        switch(opcion) {
            case 1:
                if (IniciarSesionAdmin()) {
                    printf(MENU_SUCCESS "Acceso concedido\n" COLOR_RESET);
                    menuAdmin();
                } else {
                    printf(MENU_ERROR "Acceso denegado\n" COLOR_RESET);
                }
                break;

            case 2:
                menuCliente();
                break;

            case 4:
                probarEstructuras();
                break;

            case 5:
                printf("Saliendo...\n");
                break;

            default:
                printf(MENU_ERROR "Opcion invalida\n" COLOR_RESET);
        }
    } while(opcion != 5);

    liberarUsuarios();
    liberarSitios();
    liberarEventos();
    return 0;
}