#include <stdio.h>
#include <time.h>  
#include "cliente.h"
#include "../include/colors.h"
#include "../include/evento.h"
#include "../include/sitio.h"
#include "../include/sector.h"

void comprarBoletos() {
    int eventoIdx;

    printf(MENU_TITLE "\n===== COMPRA DE BOLETOS =====\n" RESET);

    mostrarEventos();

    printf(MENU_INPUT "Seleccione evento: " RESET);
    if (scanf("%d", &eventoIdx) != 1 || eventoIdx < 1 || eventoIdx > totalEventos) {
        printf(MSG_ERROR "Evento invalido.\n" RESET);
        while(getchar() != '\n');
        return;
    }

    printf(MSG_SUCCESS "Evento seleccionado correctamente.\n" RESET);

    Evento *evento = &eventos[eventoIdx - 1];

    int sectorIdx = seleccionarSector(evento);
    if (sectorIdx == -1) return;

    int asientosSeleccionados[100];
    int cantidadAsientos = seleccionarAsientos(evento, sectorIdx, asientosSeleccionados);

    if (cantidadAsientos == 0) {
        printf(MSG_ERROR "No selecciono asientos.\n" RESET);
        return;
    }

char cedula[50], nombre[100];

printf(MENU_TITLE "\n===== DATOS DEL COMPRADOR =====\n" RESET);

printf(MENU_INPUT "Ingrese cedula: " RESET);
scanf("%s", cedula);

printf(MENU_INPUT "Ingrese nombre: " RESET);
scanf(" %[^\n]", nombre);

printf(MSG_SUCCESS "Datos registrados correctamente.\n" RESET);
    // ===== CALCULOS =====
    float subtotal = calcularSubtotal(evento, sectorIdx, asientosSeleccionados, cantidadAsientos);
    float servicio = subtotal * 0.05;
    float total = subtotal + servicio;

    // ===== FACTURA =====
    mostrarFactura(evento, cedula, nombre, subtotal, servicio, total);
guardarFactura(evento, cedula, nombre, subtotal, servicio, total);
    // ===== MARCAR ASIENTOS =====
    for (int i = 0; i < cantidadAsientos; i++) {
        marcarVendido(evento, sectorIdx, asientosSeleccionados[i]);
    }
}

int seleccionarSector(Evento *evento) {
    int sectorIdx;

    printf(MENU_TITLE "\n===== SECTORES =====\n" RESET);
    printf(MSG_INFO "Sectores disponibles:\n" RESET);

    for (int i = 0; i < evento->sitio->totalSectores; i++) {
        printf(MENU_OPTION "%d. %s\n" RESET, i + 1, evento->sitio->sectores[i].nombre);
    }

    printf(MENU_INPUT "Seleccione sector: " RESET);
    scanf("%d", &sectorIdx);

    if (sectorIdx < 1 || sectorIdx > evento->sitio->totalSectores) {
        printf(MSG_ERROR "Sector invalido.\n" RESET);
        return -1;
    }

    printf(MSG_SUCCESS "Sector seleccionado.\n" RESET);

    return sectorIdx - 1;
}

int seleccionarAsientos(Evento *evento, int sectorIdx, int seleccionados[]) {
    int cantidad = 0;
    int opcion;

    printf(MENU_TITLE "\n===== ASIENTOS =====\n" RESET);

    while (1) {

        int total = evento->sitio->sectores[sectorIdx].cantidadEspacios;
        char inicial = evento->sitio->sectores[sectorIdx].nombre[0];
        printf(WHITE "\n==========================================================\n" RESET);
        printf(MSG_INFO "Estado de asientos:\n" RESET);

        for (int i = 0; i < total; i++) {
            printf("%2d. %c%d - ", i + 1, inicial, i + 1);

            if (evento->disponibilidad[sectorIdx][i]) {
                printf(GREEN "Disponible" RESET "\n");
            } else {
                printf(RED "Vendido" RESET "\n");
            }
        }

        printf(RED "\n0. Terminar de comprar\n\n" RESET);
        printf(MENU_INPUT "Seleccione asiento: " RESET);
        scanf("%d", &opcion);

        if (opcion == 0) {
        printf(MSG_WARNING "Finalizando seleccion.\n" RESET);
        break;
}

if (!verificarDisponibilidad(evento, sectorIdx, opcion - 1)) {
    printf(MSG_ERROR "Asiento no disponible.\n" RESET);
    continue;
}

marcarVendido(evento, sectorIdx, opcion - 1);

seleccionados[cantidad++] = opcion - 1;

printf(MSG_SUCCESS "Asiento agregado.\n" RESET);
    }

    return cantidad;
}


float calcularSubtotal(Evento *evento, int sectorIdx, int asientos[], int cantidad) {
    float subtotal = 0.0;

    for (int i = 0; i < cantidad; i++) {
        subtotal += obtenerPrecioAsiento(evento, sectorIdx, asientos[i]);
    }

    return subtotal;
}
void mostrarFactura(Evento *evento, char cedula[], char nombre[], float subtotal, float servicio, float total) {

    printf("\n" MENU_BORDER "=================================\n" RESET);
    printf(MENU_TITLE "           FACTURA\n" RESET);
    printf(MENU_BORDER "=================================\n" RESET);

    printf(MAGENTA "Cedula: " RESET "%s\n", cedula);
    printf(MAGENTA "Nombre: " RESET "%s\n", nombre);
    printf(MAGENTA "Evento: " RESET "%s\n", evento->nombre);
    printf(MAGENTA "Productora: " RESET "%s\n", evento->productora);
    printf(MAGENTA "Sitio: " RESET "%s\n", evento->sitio->nombre);
    printf(MAGENTA "Fecha: " RESET "%s\n", evento->fecha);

    printf(MENU_BORDER "---------------------------------\n" RESET);

    printf(MSG_INFO BOLD "Subtotal: " RESET "%.2f\n", subtotal);
    printf(MSG_WARNING BOLD "Servicio (5%%): " RESET "%.2f\n", servicio);
    printf(MSG_SUCCESS BOLD "TOTAL: " RESET "%.2f\n", total);

    printf(MENU_BORDER "=================================\n" RESET);
}

void guardarFactura(Evento *evento, char cedula[], char nombre[], float subtotal, float servicio, float total) {

    FILE *archivo = fopen("data/facturas.txt", "a");

    if (archivo == NULL) {
        printf(MSG_ERROR "Error al abrir archivo de facturas.\n" RESET);
        return;
    }

    fprintf(archivo, "\n========== FACTURA ==========\n");
    fprintf(archivo, "Cedula: %s\n", cedula);
    fprintf(archivo, "Nombre: %s\n", nombre);
    fprintf(archivo, "Evento: %s\n", evento->nombre);
    fprintf(archivo, "Productora: %s\n", evento->productora);
    fprintf(archivo, "Sitio: %s\n", evento->sitio->nombre);
    fprintf(archivo, "Fecha evento: %s\n", evento->fecha);

    fprintf(archivo, "\nSubtotal: %.2f\n", subtotal);
    fprintf(archivo, "Servicio (5%%): %.2f\n", servicio);
    fprintf(archivo, "TOTAL: %.2f\n", total);

    fprintf(archivo, "=============================\n");

    fclose(archivo);
}


// ==================== CONSULTA POR EVENTO ====================


int compararFechas(const char *fecha1, const char *fecha2) {
    int d1, m1, a1, d2, m2, a2;
    sscanf(fecha1, "%d/%d/%d", &d1, &m1, &a1);
    sscanf(fecha2, "%d/%d/%d", &d2, &m2, &a2);
    
    if (a1 != a2) return a1 > a2;
    if (m1 != m2) return m1 > m2;
    return d1 >= d2;
}

// Función auxiliar para convertir fecha a timestamp 
time_t fechaATimestamp(const char *fecha) {
    int d, m, a;
    sscanf(fecha, "%d/%d/%d", &d, &m, &a);
    struct tm tm = {0};
    tm.tm_mday = d;
    tm.tm_mon = m - 1;
    tm.tm_year = a - 1900;
    return mktime(&tm);
}

void consultarEventos() {
    char fechaInicial[20];
    
    printf(MENU_TITLE "\n===== CONSULTA DE EVENTOS =====\n" RESET);
    printf(MENU_INPUT "Ingrese fecha inicial (dd/mm/aaaa): " RESET);
    scanf("%s", fechaInicial);
    
    // Validar formato básico
    int d, m, a;
    if (sscanf(fechaInicial, "%d/%d/%d", &d, &m, &a) != 3) {
        printf(MSG_ERROR "Formato de fecha invalido. Use dd/mm/aaaa\n" RESET);
        return;
    }
    
    // Mostrar eventos futuros
    printf("\n" MENU_BORDER "========================================\n" RESET);
    printf(MENU_TITLE "      EVENTOS FUTUROS (desde %s)\n" RESET, fechaInicial);
    printf(MENU_BORDER "========================================\n" RESET);
    
    int eventosFuturos[100];
    int countFuturos = 0;
    
    for (int i = 0; i < totalEventos; i++) {
        if (compararFechas(eventos[i].fecha, fechaInicial)) {
            eventosFuturos[countFuturos] = i;
            printf("%d. %s - %s\n", countFuturos + 1, 
                   eventos[i].nombre, eventos[i].fecha);
            countFuturos++;
        }
    }
    
    if (countFuturos == 0) {
        printf(MSG_WARNING "No hay eventos futuros desde la fecha indicada.\n" RESET);
        return;
    }
    
    // Seleccionar evento
    int seleccion;
    printf(MENU_INPUT "\nSeleccione un evento (1-%d): " RESET, countFuturos);
    scanf("%d", &seleccion);
    
    if (seleccion < 1 || seleccion > countFuturos) {
        printf(MSG_ERROR "Seleccion invalida.\n" RESET);
        return;
    }
    
    Evento *evento = &eventos[eventosFuturos[seleccion - 1]];
    
    mostrarDetallesEvento(evento);
}

void mostrarDetallesEvento(Evento *evento) {
    printf("\n" MENU_BORDER "========================================\n" RESET);
    printf(MENU_TITLE "         DETALLES DEL EVENTO\n" RESET);
    printf(MENU_BORDER "========================================\n" RESET);
    
    printf(COLOR_EVENTO BOLD "Nombre: " RESET "%s\n", evento->nombre);
    printf(COLOR_EVENTO BOLD "Productora: " RESET "%s\n", evento->productora);
    printf(COLOR_EVENTO BOLD "Sitio: " RESET "%s\n", evento->sitio->nombre);
    printf(COLOR_EVENTO BOLD "Fecha: " RESET "%s\n", evento->fecha);
    
    printf(MENU_BORDER "----------------------------------------\n" RESET);
    printf(COLOR_SITIO BOLD "SECTORES:\n" RESET);
    printf(MENU_BORDER "----------------------------------------\n" RESET);
    
    for (int s = 0; s < evento->sitio->totalSectores; s++) {
        Sector *sector = &evento->sitio->sectores[s];
        int asientosDisponibles = 0;
        
        // Contar asientos disponibles
        for (int a = 0; a < sector->cantidadEspacios; a++) {
            if (evento->disponibilidad[s][a] == 1) {
                asientosDisponibles++;
            }
        }
        
        printf(COLOR_SITIO "\n Sector: %s\n" RESET, sector->nombre);
        printf("   %-20s %-15s\n", "Monto por asiento:", "");
        printf("   %-20s " COLOR_ASIENTO "%.2f\n" RESET, "", evento->precios[s].precio);
        printf("   %-20s ", "Asientos disponibles:");
        
        if (asientosDisponibles > 0) {
            printf(GREEN "%d\n" RESET, asientosDisponibles);
        } else {
            printf(RED "%d (AGOTADOS)\n" RESET, asientosDisponibles);
        }
    }
    
    printf(MENU_BORDER "========================================\n" RESET);
}

void mostrarDetallesEventoTabla(Evento *evento) {
    printf("\n" MENU_BORDER "========================================\n" RESET);
    printf(MENU_TITLE "         DETALLES DEL EVENTO\n" RESET);
    printf(MENU_BORDER "========================================\n" RESET);
    
    printf(COLOR_EVENTO BOLD "Nombre:      " RESET "%s\n", evento->nombre);
    printf(COLOR_EVENTO BOLD "Productora:  " RESET "%s\n", evento->productora);
    printf(COLOR_EVENTO BOLD "Sitio:       " RESET "%s\n", evento->sitio->nombre);
    printf(COLOR_EVENTO BOLD "Fecha:       " RESET "%s\n", evento->fecha);
    
    printf(MENU_BORDER "----------------------------------------\n" RESET);
    printf("%-20s %-20s %-20s\n", "Sector", "Precio por asiento", "Disponibles");
    printf(MENU_BORDER "----------------------------------------\n" RESET);
    
    for (int s = 0; s < evento->sitio->totalSectores; s++) {
        Sector *sector = &evento->sitio->sectores[s];
        int asientosDisponibles = 0;
        
        for (int a = 0; a < sector->cantidadEspacios; a++) {
            if (evento->disponibilidad[s][a] == 1) {
                asientosDisponibles++;
            }
        }
        
        printf("%-20s %-20.2f ", sector->nombre, evento->precios[s].precio);
        
        if (asientosDisponibles > 0) {
            printf(GREEN "%-20d\n" RESET, asientosDisponibles);
        } else {
            printf(RED "%-20d (AGOTADOS)\n" RESET, asientosDisponibles);
        }
    }
    
    printf(MENU_BORDER "========================================\n" RESET);
}

void menuCliente() {
    char nombre[50];

    printf("\n" MENU_INPUT "Nombre del cliente: " RESET);
    scanf("%49s", nombre);

    printf(MSG_SUCCESS "Bienvenid@ %s\n" RESET, nombre);

    int opcion;

    do {
        printf("\n" MENU_BORDER "=================================\n" RESET);
        printf(MENU_TITLE "          MENU CLIENTE\n" RESET);
        printf(MENU_BORDER "=================================\n" RESET);

        printf(MENU_OPTION "1. Consultar eventos\n" RESET);
        printf(MENU_OPTION "2. Comprar boletos\n" RESET);
        printf(MENU_OPTION "3. Volver\n" RESET);

        printf(MENU_BORDER "=================================\n" RESET);

        printf(MENU_INPUT "Seleccione: " RESET);
        scanf("%d", &opcion);

        switch(opcion) {
            case 1:
                printf(MENU_BORDER "=================================\n" RESET);
                consultarEventos();
                break;

            case 2:
                printf(MENU_BORDER "=================================\n" RESET);
                printf(MSG_INFO "Aqui compras boletos...\n" RESET);
                comprarBoletos();
                break;

            case 3:
                printf(MSG_INFO "Volviendo...\n" RESET);
                break;

            default:
                printf(MSG_ERROR "Opcion invalida\n" RESET);
        }

    } while(opcion != 3);
}