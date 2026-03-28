#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <string.h>
#include "cliente.h"
#include "../include/colors.h"
#include "../include/evento.h"
#include "../include/sitio.h"
#include "../include/sector.h"
#include "../include/factura.h"
#include "../include/pdf_factura.h"

/*
 * Objetivo: Indicar si un anio es bisiesto bajo reglas del calendario gregoriano.
 * Entradas: anio numerico.
 * Salidas: 1 si es bisiesto, 0 en caso contrario.
 * Restricciones: espera anios positivos dentro de rango razonable.
 */
static int esBisiesto(int anio)
{
    return (anio % 400 == 0) || (anio % 4 == 0 && anio % 100 != 0);
}

/*
 * Objetivo: Copiar un mensaje de error en buffer con proteccion de limites.
 * Entradas: buffer destino, tamanio del buffer y texto origen.
 * Salidas: escribe el texto cuando el destino es valido.
 * Restricciones: tam debe ser mayor a 0 para copiar.
 */
static void setMensajeError(char *mensaje, size_t tam, const char *texto)
{
    if (mensaje != NULL && tam > 0)
    {
        snprintf(mensaje, tam, "%s", texto);
    }
}

/*
 * Objetivo: Validar fecha de entrada y generar detalle de error en caso invalido.
 * Entradas: fecha en formato texto, buffer de mensaje y tamanio.
 * Salidas: 1 si fecha valida, 0 si hay error.
 * Restricciones: formato esperado dd/mm/aaaa.
 */
static int validarFechaConDetalle(const char *fecha, char *mensaje, size_t tam)
{
    int d, m, a;
    int diasMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (sscanf(fecha, "%d/%d/%d", &d, &m, &a) != 3)
    {
        setMensajeError(mensaje, tam, "formato invalido; use dd/mm/aaaa (ejemplo: 28/03/2026)");
        return 0;
    }

    if (a < 1900)
    {
        setMensajeError(mensaje, tam, "anio invalido; el anio minimo permitido es 1900");
        return 0;
    }

    if (a > 9999)
    {
        setMensajeError(mensaje, tam, "anio invalido; el anio maximo permitido es 9999");
        return 0;
    }

    if (m < 1 || m > 12)
    {
        setMensajeError(mensaje, tam, "mes invalido; debe estar entre 1 y 12");
        return 0;
    }

    if (m == 2 && esBisiesto(a))
    {
        diasMes[1] = 29;
    }

    if (d < 1 || d > diasMes[m - 1])
    {
        snprintf(mensaje, tam, "dia invalido para el mes %d; rango permitido: 1-%d", m, diasMes[m - 1]);
        return 0;
    }

    return 1;
}

/*
 * Objetivo: Validar fecha de forma booleana sin mensaje detallado.
 * Entradas: fecha en texto.
 * Salidas: 1 valida, 0 invalida.
 * Restricciones: delega en validarFechaConDetalle().
 */
static int esFechaValida(const char *fecha)
{
    return validarFechaConDetalle(fecha, NULL, 0);
}

/*
 * Objetivo: Validar cedula y reportar razon de invalidez.
 * Entradas: cedula en texto, buffer de mensaje y tamanio.
 * Salidas: 1 si cumple reglas, 0 en caso contrario.
 * Restricciones: exige exactamente 9 digitos numericos.
 */
static int validarCedulaConDetalle(const char *cedula, char *mensaje, size_t tam)
{
    if (cedula == NULL)
    {
        setMensajeError(mensaje, tam, "la cedula no puede estar vacia");
        return 0;
    }

    if (strlen(cedula) != 9)
    {
        setMensajeError(mensaje, tam, "debe contener exactamente 9 digitos");
        return 0;
    }

    for (size_t i = 0; i < 9; i++)
    {
        if (!isdigit((unsigned char)cedula[i]))
        {
            snprintf(mensaje, tam, "solo se permiten digitos (caracter invalido: '%c')", cedula[i]);
            return 0;
        }
    }

    return 1;
}

/*
 * Objetivo: Validar cedula de forma booleana sin mensaje de detalle.
 * Entradas: cedula en texto.
 * Salidas: 1 valida, 0 invalida.
 * Restricciones: delega en validarCedulaConDetalle().
 */
static int esCedulaValida(const char *cedula)
{
    return validarCedulaConDetalle(cedula, NULL, 0);
}

/*
 * Objetivo: Limpiar el buffer de entrada estandar.
 * Entradas: No recibe parametros.
 * Salidas: descarta caracteres hasta fin de linea o EOF.
 * Restricciones: usar tras scanf con error de parseo.
 */
static void limpiarBufferEntrada(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

/*
 * Objetivo: Leer un entero de forma segura para menus cliente.
 * Entradas: prompt opcional y puntero destino.
 * Salidas: 1 si lectura correcta, 0 si invalida.
 * Restricciones: valor no debe ser NULL.
 */
static int leerEnteroSeguro(const char *prompt, int *valor)
{
    if (prompt != NULL)
    {
        printf("%s", prompt);
    }
    if (scanf("%d", valor) != 1)
    {
        limpiarBufferEntrada();
        return 0;
    }
    return 1;
}

/*
 * Objetivo: Construir la cadena de detalle de asientos para la factura.
 * Entradas: evento, indice de sector, arreglo de indices de asientos, cantidad, buffer destino y tamanio (tam).
 * Salidas: detalle en formato "A1(precio), A2(precio), ..." en el buffer destino.
 * Restricciones: destino debe tener al menos tam bytes de espacio disponible.
 */
static void construirDetalleAsientos(Evento *evento, int sectorIdx, int asientos[], int cantidad, char *destino, size_t tam)
{
    destino[0] = '\0';
    char inicial = evento->sitio->sectores[sectorIdx].inicial;

    for (int i = 0; i < cantidad; i++)
    {
        char item[96];
        float precio = obtenerPrecioAsiento(evento, sectorIdx, asientos[i]);
        snprintf(item, sizeof(item), "%c%d(%.2f)", inicial, asientos[i] + 1, precio);

        if (i > 0)
        {
            strncat(destino, ", ", tam - strlen(destino) - 1);
        }
        strncat(destino, item, tam - strlen(destino) - 1);
    }
}

/*
 * Objetivo: Orquestar el flujo completo de compra de boletos para un cliente.
 * Entradas: seleccion de evento/sector/asientos y datos de comprador por consola.
 * Salidas: marca asientos vendidos, genera y guarda factura.
 * Restricciones: requiere eventos y sectores cargados previamente.
 */
void comprarBoletos()
{
    int eventoIdx;

    printf(MENU_TITLE "\n===== COMPRA DE BOLETOS =====\n" RESET);

    mostrarEventos();

    if (!leerEnteroSeguro(MENU_INPUT "Seleccione evento: " RESET, &eventoIdx) || eventoIdx < 1 || eventoIdx > totalEventos)
    {
        printf(MSG_ERROR "Evento invalido.\n" RESET);
        return;
    }

    printf(MSG_SUCCESS "Evento seleccionado correctamente.\n" RESET);

    Evento *evento = &eventos[eventoIdx - 1];

    int sectorIdx = seleccionarSector(evento);
    if (sectorIdx == -1)
        return;

    int capacidadAsientos = evento->sitio->sectores[sectorIdx].cantidadEspacios;
    int *asientosSeleccionados = (int *)malloc(capacidadAsientos * sizeof(int));
    if (asientosSeleccionados == NULL)
    {
        printf(MSG_ERROR "Error de memoria al seleccionar asientos.\n" RESET);
        return;
    }
    int cantidadAsientos = seleccionarAsientos(evento, sectorIdx, asientosSeleccionados);

    if (cantidadAsientos == 0)
    {
        printf(MSG_ERROR "No selecciono asientos.\n" RESET);
        free(asientosSeleccionados);
        return;
    }

    char cedula[50], nombre[100];

    printf(MENU_TITLE "\n===== DATOS DEL COMPRADOR =====\n" RESET);

    do
    {
        char errorCedula[128];
        printf(MENU_INPUT "Ingrese cedula (9 digitos): " RESET);
        scanf("%49s", cedula);

        if (!validarCedulaConDetalle(cedula, errorCedula, sizeof(errorCedula)))
        {
            printf(MSG_ERROR "Cedula invalida: %s.\n" RESET, errorCedula);
        }
    } while (!esCedulaValida(cedula));

    printf(MENU_INPUT "Ingrese nombre: " RESET);
    scanf(" %99[^\n]", nombre);

    printf(MSG_SUCCESS "Datos registrados correctamente.\n" RESET);
    // ===== CALCULOS =====
    float subtotal = calcularSubtotal(evento, sectorIdx, asientosSeleccionados, cantidadAsientos);
    float servicio = subtotal * 0.05;
    float total = subtotal + servicio;
    char detalleAsientos[MAX_DETALLE_FACTURA];
    construirDetalleAsientos(evento, sectorIdx, asientosSeleccionados, cantidadAsientos, detalleAsientos, sizeof(detalleAsientos));

    // ===== FACTURA =====
    Factura factura;
    if (facturaCrearDesdeCompra(&factura, evento, cedula, nombre, subtotal, servicio, total, detalleAsientos, 0))
    {
        if (facturaGuardar(ARCHIVO_FACTURAS, &factura))
        {
            char rutaPDF[256];

            facturaMostrarDetalle(&factura);
            if (facturaGenerarPDF(&factura, FACTURA_PDF_DIR, rutaPDF, sizeof(rutaPDF)))
            {
                printf(MSG_SUCCESS "Factura PDF generada en: %s\n" RESET, rutaPDF);
            }
            else
            {
                printf(MSG_WARNING "No se pudo generar la factura en PDF.\n" RESET);
            }
        }
        else
        {
            printf(MSG_ERROR "No se pudo guardar la factura en archivo.\n" RESET);
        }
    }

    free(asientosSeleccionados);
}

/*
 * Objetivo: Permitir al cliente elegir un sector de un evento.
 * Entradas: puntero al evento.
 * Salidas: indice de sector (base 0) o -1 si hay error.
 * Restricciones: evento debe tener al menos un sector.
 */
int seleccionarSector(Evento *evento)
{
    int sectorIdx;

    printf(MENU_TITLE "\n===== SECTORES =====\n" RESET);
    printf(MSG_INFO "Sectores disponibles:\n" RESET);

    for (int i = 0; i < evento->sitio->totalSectores; i++)
    {
        printf(MENU_OPTION "%d. %s\n" RESET, i + 1, evento->sitio->sectores[i].nombre);
    }

    if (!leerEnteroSeguro(MENU_INPUT "Seleccione sector: " RESET, &sectorIdx))
    {
        printf(MSG_ERROR "Entrada invalida.\n" RESET);
        return -1;
    }

    if (sectorIdx < 1 || sectorIdx > evento->sitio->totalSectores)
    {
        printf(MSG_ERROR "Sector invalido.\n" RESET);
        return -1;
    }

    printf(MSG_SUCCESS "Sector seleccionado.\n" RESET);

    return sectorIdx - 1;
}

/*
 * Objetivo: Registrar seleccion de asientos disponibles para un sector.
 * Entradas: evento, indice de sector y arreglo de salida seleccionados.
 * Salidas: cantidad de asientos elegidos.
 * Restricciones: solo agrega asientos disponibles y en rango valido.
 */
int seleccionarAsientos(Evento *evento, int sectorIdx, int seleccionados[])
{
    int cantidad = 0;
    int opcion;

    printf(MENU_TITLE "\n===== ASIENTOS =====\n" RESET);

    while (1)
    {

        int total = evento->sitio->sectores[sectorIdx].cantidadEspacios;
        char inicial = evento->sitio->sectores[sectorIdx].nombre[0];
        printf(WHITE "\n==========================================================\n" RESET);
        printf(MSG_INFO "Estado de asientos:\n" RESET);

        for (int i = 0; i < total; i++)
        {
            printf("%2d. %c%d - ", i + 1, inicial, i + 1);

            if (evento->disponibilidad[sectorIdx][i])
            {
                printf(GREEN "Disponible" RESET "\n");
            }
            else
            {
                printf(RED "Vendido" RESET "\n");
            }
        }

        printf(RED "\n0. Terminar de comprar\n\n" RESET);
        if (!leerEnteroSeguro(MENU_INPUT "Seleccione asiento: " RESET, &opcion))
        {
            printf(MSG_ERROR "Entrada invalida.\n" RESET);
            continue;
        }

        if (opcion == 0)
        {
            printf(MSG_WARNING "Finalizando seleccion.\n" RESET);
            break;
        }

        if (opcion < 1 || opcion > total)
        {
            printf(MSG_ERROR "Asiento invalido.\n" RESET);
            continue;
        }

        if (!verificarDisponibilidad(evento, sectorIdx, opcion - 1))
        {
            printf(MSG_ERROR "Asiento no disponible.\n" RESET);
            continue;
        }

        marcarVendido(evento, sectorIdx, opcion - 1);

        if (cantidad < total)
        {
            seleccionados[cantidad++] = opcion - 1;
        }

        printf(MSG_SUCCESS "Asiento agregado.\n" RESET);
    }

    return cantidad;
}

/*
 * Objetivo: Calcular subtotal de compra segun asientos seleccionados.
 * Entradas: evento, sector y arreglo/cantidad de asientos.
 * Salidas: monto subtotal en float.
 * Restricciones: asientos deben corresponder al sector indicado.
 */
float calcularSubtotal(Evento *evento, int sectorIdx, int asientos[], int cantidad)
{
    float subtotal = 0.0;

    for (int i = 0; i < cantidad; i++)
    {
        subtotal += obtenerPrecioAsiento(evento, sectorIdx, asientos[i]);
    }

    return subtotal;
}
// ==================== CONSULTA POR EVENTO ====================

/*
 * Objetivo: Comparar dos fechas y determinar si fecha1 es igual o posterior a fecha2.
 * Entradas: fecha1 y fecha2 en formato dd/mm/aaaa.
 * Salidas: 1 si fecha1 >= fecha2, 0 en caso contrario.
 * Restricciones: asume formato correcto en ambas fechas.
 */
int compararFechas(const char *fecha1, const char *fecha2)
{
    int d1, m1, a1, d2, m2, a2;
    sscanf(fecha1, "%d/%d/%d", &d1, &m1, &a1);
    sscanf(fecha2, "%d/%d/%d", &d2, &m2, &a2);

    if (a1 != a2)
        return a1 > a2;
    if (m1 != m2)
        return m1 > m2;
    return d1 >= d2;
}

/*
 * Objetivo: Convertir una fecha textual a timestamp del sistema.
 * Entradas: fecha en formato dd/mm/aaaa.
 * Salidas: time_t correspondiente a la fecha.
 * Restricciones: comportamiento indefinido si la fecha no cumple formato esperado.
 */
time_t fechaATimestamp(const char *fecha)
{
    int d, m, a;
    sscanf(fecha, "%d/%d/%d", &d, &m, &a);
    struct tm tm = {0};
    tm.tm_mday = d;
    tm.tm_mon = m - 1;
    tm.tm_year = a - 1900;
    return mktime(&tm);
}

/*
 * Objetivo: Consultar eventos futuros a partir de una fecha inicial ingresada.
 * Entradas: fecha inicial y seleccion de evento por consola.
 * Salidas: muestra lista de eventos futuros y detalle del evento elegido.
 * Restricciones: requiere al menos un evento que cumpla el filtro.
 */
void consultarEventos()
{
    char fechaInicial[20];
    char detalleErrorFecha[160];

    printf(MENU_TITLE "\n===== CONSULTA DE EVENTOS =====\n" RESET);
    printf(MENU_INPUT "Ingrese fecha inicial (dd/mm/aaaa): " RESET);
    if (scanf("%19s", fechaInicial) != 1)
    {
        printf(MSG_ERROR "No se pudo leer la fecha. Use el formato dd/mm/aaaa.\n" RESET);
        limpiarBufferEntrada();
        return;
    }

    if (!validarFechaConDetalle(fechaInicial, detalleErrorFecha, sizeof(detalleErrorFecha)))
    {
        printf(MSG_ERROR "Fecha invalida: %s.\n" RESET, detalleErrorFecha);
        return;
    }

    // Mostrar eventos futuros
    printf("\n" MENU_BORDER "========================================\n" RESET);
    printf(MENU_TITLE "      EVENTOS FUTUROS (desde %s)\n" RESET, fechaInicial);
    printf(MENU_BORDER "========================================\n" RESET);

    int *eventosFuturos = (int *)malloc((size_t)totalEventos * sizeof(int));
    if (eventosFuturos == NULL)
    {
        printf(MSG_ERROR "Error de memoria al listar eventos.\n" RESET);
        return;
    }
    int countFuturos = 0;

    for (int i = 0; i < totalEventos; i++)
    {
        if (compararFechas(eventos[i].fecha, fechaInicial))
        {
            eventosFuturos[countFuturos] = i;
            printf("%d. %s - %s\n", countFuturos + 1,
                   eventos[i].nombre, eventos[i].fecha);
            countFuturos++;
        }
    }

    if (countFuturos == 0)
    {
        printf(MSG_WARNING "No hay eventos futuros desde la fecha indicada.\n" RESET);
        free(eventosFuturos);
        return;
    }

    // Seleccionar evento
    int seleccion;
    printf(MENU_INPUT "\nSeleccione un evento (1-%d): " RESET, countFuturos);
    if (!leerEnteroSeguro(NULL, &seleccion))
    {
        printf(MSG_ERROR "Seleccion invalida.\n" RESET);
        free(eventosFuturos);
        return;
    }

    if (seleccion < 1 || seleccion > countFuturos)
    {
        printf(MSG_ERROR "Seleccion invalida.\n" RESET);
        free(eventosFuturos);
        return;
    }

    Evento *evento = &eventos[eventosFuturos[seleccion - 1]];
    free(eventosFuturos);

    mostrarDetallesEvento(evento);
}

/*
 * Objetivo: Mostrar detalle de un evento con precios por sector y disponibilidad.
 * Entradas: puntero al evento a mostrar.
 * Salidas: impresion formateada en consola.
 * Restricciones: evento y sus estructuras internas deben ser validos.
 */
void mostrarDetallesEvento(Evento *evento)
{
    printf("\n" MENU_BORDER "========================================\n" RESET);
    printf(MENU_TITLE "         DETALLES DEL EVENTO\n" RESET);
    printf(MENU_BORDER "========================================\n" RESET);

    printf(COLOR_EVENTO BOLD "Nombre: " RESET "%s\n", evento->nombre);
    printf(COLOR_EVENTO BOLD "Productora: " RESET "%s\n", evento->productora);
    printf(COLOR_EVENTO BOLD "Sitio: " RESET "%s\n", evento->sitio->nombre);
    printf(COLOR_EVENTO BOLD "Fecha evento: " RESET "%s\n", evento->fecha);

    printf(MENU_BORDER "----------------------------------------\n" RESET);
    printf(COLOR_SITIO BOLD "SECTORES:\n" RESET);
    printf(MENU_BORDER "----------------------------------------\n" RESET);

    for (int s = 0; s < evento->sitio->totalSectores; s++)
    {
        Sector *sector = &evento->sitio->sectores[s];
        int asientosDisponibles = 0;

        // Contar asientos disponibles
        for (int a = 0; a < sector->cantidadEspacios; a++)
        {
            if (evento->disponibilidad[s][a] == 1)
            {
                asientosDisponibles++;
            }
        }

        printf(COLOR_SITIO "\n Sector: %s\n" RESET, sector->nombre);
        printf("   %-20s %-15s\n", "Monto por asiento:", "");
        printf("   %-20s " COLOR_ASIENTO "%.2f\n" RESET, "", evento->precios[s].precio);
        printf("   %-20s ", "Asientos disponibles:");

        if (asientosDisponibles > 0)
        {
            printf(GREEN "%d\n" RESET, asientosDisponibles);
        }
        else
        {
            printf(RED "%d (AGOTADOS)\n" RESET, asientosDisponibles);
        }
    }

    printf(MENU_BORDER "========================================\n" RESET);
}

/*
 * Objetivo: Mostrar detalle tabular alternativo de un evento.
 * Entradas: puntero al evento a mostrar.
 * Salidas: tabla con sector, precio y asientos disponibles.
 * Restricciones: funcion de apoyo visual; no altera estado del sistema.
 */
void mostrarDetallesEventoTabla(Evento *evento)
{
    printf("\n" MENU_BORDER "========================================\n" RESET);
    printf(MENU_TITLE "         DETALLES DEL EVENTO\n" RESET);
    printf(MENU_BORDER "========================================\n" RESET);

    printf(COLOR_EVENTO BOLD "Nombre:      " RESET "%s\n", evento->nombre);
    printf(COLOR_EVENTO BOLD "Productora:  " RESET "%s\n", evento->productora);
    printf(COLOR_EVENTO BOLD "Sitio:       " RESET "%s\n", evento->sitio->nombre);
    printf(COLOR_EVENTO BOLD "Fecha evento:" RESET " %s\n", evento->fecha);

    printf(MENU_BORDER "----------------------------------------\n" RESET);
    printf("%-20s %-20s %-20s\n", "Sector", "Precio por asiento", "Disponibles");
    printf(MENU_BORDER "----------------------------------------\n" RESET);

    for (int s = 0; s < evento->sitio->totalSectores; s++)
    {
        Sector *sector = &evento->sitio->sectores[s];
        int asientosDisponibles = 0;

        for (int a = 0; a < sector->cantidadEspacios; a++)
        {
            if (evento->disponibilidad[s][a] == 1)
            {
                asientosDisponibles++;
            }
        }

        printf("%-20s %-20.2f ", sector->nombre, evento->precios[s].precio);

        if (asientosDisponibles > 0)
        {
            printf(GREEN "%-20d\n" RESET, asientosDisponibles);
        }
        else
        {
            printf(RED "%-20d (AGOTADOS)\n" RESET, asientosDisponibles);
        }
    }

    printf(MENU_BORDER "========================================\n" RESET);
}

/*
 * Objetivo: Gestionar el menu de opciones para clientes.
 * Entradas: opcion seleccionada por consola.
 * Salidas: ejecuta consulta, compra o retorno al menu principal.
 * Restricciones: el bucle finaliza cuando opcion es 3.
 */
void menuCliente()
{
    printf(MSG_SUCCESS "Bienvenid@ a la seccion de cliente\n" RESET);

    int opcion;

    do
    {
        printf("\n" MENU_BORDER "=================================\n" RESET);
        printf(MENU_TITLE "          MENU CLIENTE\n" RESET);
        printf(MENU_BORDER "=================================\n" RESET);

        printf(MENU_OPTION "1. Consultar eventos\n" RESET);
        printf(MENU_OPTION "2. Comprar boletos\n" RESET);
        printf(MENU_OPTION "3. Volver\n" RESET);

        printf(MENU_BORDER "=================================\n" RESET);

        if (!leerEnteroSeguro(MENU_INPUT "Seleccione: " RESET, &opcion))
        {
            printf(MSG_ERROR "Entrada invalida\n" RESET);
            continue;
        }

        switch (opcion)
        {
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

    } while (opcion != 3);
}