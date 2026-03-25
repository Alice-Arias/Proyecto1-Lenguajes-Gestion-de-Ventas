#include <stdio.h>
#include "admin/admin.h"
#include "cliente/cliente.h"
#include "include/sitio.h"
#include "include/evento.h"

void probarEstructuras() {
    printf("\n=== Probando estructuras ===\n");

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

    printf("\n=== Fin de prueba ===\n");
}

int main() {

    cargarUsuarios(); // cargar datos a memoria
    inicializarSitios();
    inicializarEventos();

    int opcion;

    do {
        printf("\n========= SISTEMA DE BOLETOS =========\n");
        printf("1. Administrador\n");
        printf("2. Cliente\n");
        printf("4. Probar Estructuras\n");
        printf("5. Salir\n");
        printf("======================================\n");


        printf("Seleccione: ");
        scanf("%d", &opcion);

        switch(opcion) {

            case 1:
                if (IniciarSesionAdmin()) {
                    printf("Acceso concedido\n");
                    menuAdmin();
                } else {
                    printf("Acceso denegado\n");
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
                printf("Opcion invalida\n");
        }

    } while(opcion != 5);

    liberarUsuarios();
    liberarSitios();
    liberarEventos();

    return 0;
}