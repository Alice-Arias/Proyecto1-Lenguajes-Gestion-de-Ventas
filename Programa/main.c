#include <stdio.h>
#include "admin/admin.h"
#include "cliente/cliente.h"
#include "include/sitio.h"
#include "include/evento.h"
#include "include/sector.h"
#include "include/asiento.h"
#include "include/factura.h"
#include "include/colors.h"

int main()
{
    cargarUsuarios();
    inicializarSitios();
    cargarSitiosDesdeArchivo("data/sitios.txt");
    cargarSectoresDesdeArchivo("data/sector.txt");
    inicializarEventos();
    cargarEventosDesdeArchivo("data/evento.txt");
    cargarAsientosDesdeArchivo("data/asientos.txt");
    sincronizarAsientosConFacturas("data/facturas.txt");
    guardarAsientosEnArchivo("data/asientos.txt");
    int opcion;

    do
    {
        printf("\n" MENU_BORDER "======================================\n" RESET);
        printf(MENU_TITLE "        SISTEMA DE BOLETOS\n" RESET);
        printf(MENU_BORDER "======================================\n" RESET);

        printf(MENU_OPTION "1. Administrador\n" RESET);
        printf(MENU_OPTION "2. Cliente\n" RESET);
        printf(MENU_OPTION "3. Salir\n" RESET);

        printf(MENU_BORDER "======================================\n" RESET);

        printf(MENU_INPUT "Seleccione: " RESET);
        scanf("%d", &opcion);

        switch (opcion)
        {

        case 1:
            if (IniciarSesionAdmin())
            {
                printf(MSG_SUCCESS "Acceso concedido\n" RESET);
                menuAdmin();
            }
            else
            {
                printf(MSG_ERROR "Acceso denegado\n" RESET);
            }
            break;

        case 2:
            menuCliente();
            break;

        case 3:
            printf(MSG_WARNING "Saliendo del sistema...\n" RESET);
            break;

        default:
            printf(MSG_ERROR "Opcion invalida\n" RESET);
        }

    } while (opcion != 3);

    guardarSitiosEnArchivo("data/sitios.txt");
    guardarSectoresEnArchivo("data/sector.txt");
    guardarEventosEnArchivo("data/evento.txt");
    guardarAsientosEnArchivo("data/asientos.txt");
    liberarUsuarios();
    liberarSitios();
    liberarEventos();

    return 0;
}