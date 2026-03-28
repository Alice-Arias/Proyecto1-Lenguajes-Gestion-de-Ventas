#include "../include/factura.h"
#include "../include/colors.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*
 * Objetivo: Eliminar salto de linea CR/LF al final de una cadena leida de archivo.
 * Entradas: texto mutable terminado en '\0'.
 * Salidas: modifica la cadena in-place removiendo \r y \n finales.
 * Restricciones: texto debe ser puntero valido.
 */
static void trimNuevaLinea(char *texto)
{
    texto[strcspn(texto, "\r\n")] = '\0';
}

/*
 * Objetivo: Verificar si una cadena inicia con un prefijo dado.
 * Entradas: texto completo y prefijo a comparar.
 * Salidas: 1 si coincide al inicio, 0 en caso contrario.
 * Restricciones: texto y prefijo deben ser cadenas validas.
 */
static int startsWith(const char *texto, const char *prefijo)
{
    return strncmp(texto, prefijo, strlen(prefijo)) == 0;
}

/*
 * Objetivo: Calcular el siguiente identificador de factura disponible.
 * Entradas: ruta del archivo de facturas.
 * Salidas: retorna maxId + 1 o 1 si no hay facturas previas.
 * Restricciones: depende de que facturaCargarTodas pueda leer el archivo.
 */
static int facturaObtenerSiguienteId(const char *ruta)
{
    Factura *facturas = NULL;
    int cantidad = 0;
    int maxId = 0;

    if (facturaCargarTodas(ruta, &facturas, &cantidad) == 0)
    {
        return 1;
    }

    for (int i = 0; i < cantidad; i++)
    {
        if (facturas[i].id > maxId)
        {
            maxId = facturas[i].id;
        }
    }

    facturaLiberarLista(facturas);
    return maxId + 1;
}

/*
 * Objetivo: Construir una factura en memoria a partir de una compra realizada.
 * Entradas: estructura destino, evento, cedula, nombre de cliente, subtotal, servicio, total, detalleAsientos e idSugerido.
 * Salidas: factura inicializada; retorna 1 en exito, 0 en error.
 * Restricciones: factura, evento, cedula y cliente no deben ser NULL.
 */
int facturaCrearDesdeCompra(Factura *factura, const Evento *evento, const char *cedula, const char *cliente,
                            float subtotal, float servicio, float total, const char *detalleAsientos, int idSugerido)
{
    if (factura == NULL || evento == NULL || cedula == NULL || cliente == NULL)
    {
        return 0;
    }

    memset(factura, 0, sizeof(Factura));

    factura->id = idSugerido;
    strncpy(factura->cedula, cedula, MAX_CEDULA_FACTURA - 1);
    strncpy(factura->cliente, cliente, MAX_CLIENTE_FACTURA - 1);
    strncpy(factura->evento, evento->nombre, MAX_NOMBRE_EVENTO - 1);
    strncpy(factura->productora, evento->productora, MAX_PRODUCTORA - 1);
    strncpy(factura->sitio, evento->sitio->nombre, MAX_NOMBRE - 1);
    strncpy(factura->fechaEvento, evento->fecha, MAX_FECHA - 1);
    if (detalleAsientos != NULL && detalleAsientos[0] != '\0')
    {
        strncpy(factura->detalleAsientos, detalleAsientos, MAX_DETALLE_FACTURA - 1);
    }
    else
    {
        strncpy(factura->detalleAsientos, "N/A", MAX_DETALLE_FACTURA - 1);
    }
    factura->subtotal = subtotal;
    factura->servicio = servicio;
    factura->total = total;

    time_t ahora = time(NULL);
    struct tm *tmInfo = localtime(&ahora);
    if (tmInfo != NULL)
    {
        strftime(factura->fechaCompra, sizeof(factura->fechaCompra), "%d/%m/%Y", tmInfo);
    }
    else
    {
        strncpy(factura->fechaCompra, "N/A", MAX_FECHA - 1);
    }

    return 1;
}

/*
 * Objetivo: Mostrar en consola el detalle completo de una factura.
 * Entradas: puntero constante a factura.
 * Salidas: impresion formateada para visualizacion del cliente.
 * Restricciones: si factura es NULL la funcion finaliza sin imprimir.
 */
void facturaMostrarDetalle(const Factura *factura)
{
    if (factura == NULL)
    {
        return;
    }

    printf("\n" MENU_BORDER "=================================\n" RESET);
    printf(MENU_TITLE "           FACTURA\n" RESET);
    printf(MENU_BORDER "=================================\n" RESET);

    printf(MAGENTA "ID: " RESET "%d\n", factura->id);
    printf(MAGENTA "Cedula: " RESET "%s\n", factura->cedula);
    printf(MAGENTA "Nombre: " RESET "%s\n", factura->cliente);
    printf(MAGENTA "Evento: " RESET "%s\n", factura->evento);
    printf(MAGENTA "Productora: " RESET "%s\n", factura->productora);
    printf(MAGENTA "Sitio: " RESET "%s\n", factura->sitio);
    printf(MAGENTA "Fecha compra: " RESET "%s\n", factura->fechaCompra);
    printf(MAGENTA "Fecha evento: " RESET "%s\n", factura->fechaEvento);
    printf(MAGENTA "Detalle asientos: " RESET "%s\n", factura->detalleAsientos);

    printf(MENU_BORDER "---------------------------------\n" RESET);

    printf(MSG_INFO BOLD "Subtotal: " RESET "%.2f\n", factura->subtotal);
    printf(MSG_WARNING BOLD "Servicio (5%%): " RESET "%.2f\n", factura->servicio);
    printf(MSG_SUCCESS BOLD "TOTAL: " RESET "%.2f\n", factura->total);

    printf(MENU_BORDER "=================================\n" RESET);
}

/*
 * Objetivo: Persistir una factura en el archivo historico de facturacion.
 * Entradas: ruta destino y puntero a factura.
 * Salidas: retorna 1 si guarda correctamente, 0 si ocurre error.
 * Restricciones: si factura->id <= 0 se autogenera un nuevo identificador.
 */
int facturaGuardar(const char *ruta, Factura *factura)
{
    if (ruta == NULL || factura == NULL)
    {
        return 0;
    }

    int idFactura = factura->id;
    if (idFactura <= 0)
    {
        idFactura = facturaObtenerSiguienteId(ruta);
    }
    factura->id = idFactura;

    FILE *archivo = fopen(ruta, "a");
    if (archivo == NULL)
    {
        printf(MSG_ERROR "Error al abrir archivo de facturas.\n" RESET);
        return 0;
    }

    fprintf(archivo, "\n========== FACTURA ==========\n");
    fprintf(archivo, "ID: %d\n", idFactura);
    fprintf(archivo, "Cedula: %s\n", factura->cedula);
    fprintf(archivo, "Nombre: %s\n", factura->cliente);
    fprintf(archivo, "Evento: %s\n", factura->evento);
    fprintf(archivo, "Productora: %s\n", factura->productora);
    fprintf(archivo, "Sitio: %s\n", factura->sitio);
    fprintf(archivo, "Fecha compra: %s\n", factura->fechaCompra);
    fprintf(archivo, "Fecha evento: %s\n", factura->fechaEvento);
    fprintf(archivo, "Detalle asientos: %s\n", factura->detalleAsientos);
    fprintf(archivo, "\nSubtotal: %.2f\n", factura->subtotal);
    fprintf(archivo, "Servicio (5%%): %.2f\n", factura->servicio);
    fprintf(archivo, "TOTAL: %.2f\n", factura->total);
    fprintf(archivo, "=============================\n");

    fclose(archivo);
    return 1;
}

/*
 * Objetivo: Cargar todas las facturas de archivo a un arreglo dinamico.
 * Entradas: ruta, doble puntero de salida y contador de salida.
 * Salidas: retorna 1 en exito y llena facturas/cantidad.
 * Restricciones: el formato del archivo debe coincidir con el esperado por el parser.
 */
int facturaCargarTodas(const char *ruta, Factura **facturas, int *cantidad)
{
    if (ruta == NULL || facturas == NULL || cantidad == NULL)
    {
        return 0;
    }

    *facturas = NULL;
    *cantidad = 0;

    FILE *f = fopen(ruta, "r");
    if (!f)
    {
        return 0;
    }

    char linea[2048];
    Factura actual;
    int enFactura = 0;
    int contador = 1;

    memset(&actual, 0, sizeof(Factura));

    while (fgets(linea, sizeof(linea), f))
    {
        trimNuevaLinea(linea);

        if (strstr(linea, "========== FACTURA ==========") != NULL)
        {
            memset(&actual, 0, sizeof(Factura));
            actual.id = contador;
            strcpy(actual.fechaCompra, "N/A");
            strcpy(actual.fechaEvento, "N/A");
            strcpy(actual.detalleAsientos, "N/A");
            enFactura = 1;
            continue;
        }

        if (!enFactura)
        {
            continue;
        }

        if (strstr(linea, "=============================") != NULL)
        {
            Factura *tmp = realloc(*facturas, (*cantidad + 1) * sizeof(Factura));
            if (tmp == NULL)
            {
                free(*facturas);
                *facturas = NULL;
                *cantidad = 0;
                fclose(f);
                return 0;
            }
            *facturas = tmp;
            (*facturas)[*cantidad] = actual;
            (*cantidad)++;
            contador++;
            enFactura = 0;
            continue;
        }

        if (startsWith(linea, "ID:"))
        {
            sscanf(linea, "ID: %d", &actual.id);
        }
        else if (startsWith(linea, "Cedula:"))
        {
            sscanf(linea, "Cedula: %49[^\n]", actual.cedula);
        }
        else if (startsWith(linea, "Nombre:"))
        {
            sscanf(linea, "Nombre: %99[^\n]", actual.cliente);
        }
        else if (startsWith(linea, "Evento:"))
        {
            sscanf(linea, "Evento: %99[^\n]", actual.evento);
        }
        else if (startsWith(linea, "Productora:"))
        {
            sscanf(linea, "Productora: %99[^\n]", actual.productora);
        }
        else if (startsWith(linea, "Sitio:"))
        {
            sscanf(linea, "Sitio: %49[^\n]", actual.sitio);
        }
        else if (startsWith(linea, "Fecha compra:"))
        {
            sscanf(linea, "Fecha compra: %10[^\n]", actual.fechaCompra);
        }
        else if (startsWith(linea, "Fecha evento:"))
        {
            sscanf(linea, "Fecha evento: %10[^\n]", actual.fechaEvento);
        }
        else if (startsWith(linea, "Detalle asientos:"))
        {
            sscanf(linea, "Detalle asientos: %2047[^\n]", actual.detalleAsientos);
        }
        else if (startsWith(linea, "Subtotal:"))
        {
            sscanf(linea, "Subtotal: %f", &actual.subtotal);
        }
        else if (startsWith(linea, "Servicio (5%):"))
        {
            sscanf(linea, "Servicio (5%%): %f", &actual.servicio);
        }
        else if (startsWith(linea, "TOTAL:"))
        {
            sscanf(linea, "TOTAL: %f", &actual.total);
        }
    }

    fclose(f);
    return 1;
}

/*
 * Objetivo: Liberar el arreglo dinamico de facturas cargadas.
 * Entradas: puntero al arreglo de facturas.
 * Salidas: memoria liberada.
 * Restricciones: permite recibir NULL de forma segura.
 */
void facturaLiberarLista(Factura *facturas)
{
    free(facturas);
}

/*
 * Objetivo: Mostrar en consola el listado resumido de facturas para administracion.
 * Entradas: ruta del archivo de facturas.
 * Salidas: tabla con ID, evento, fecha, cliente y subtotal.
 * Restricciones: requiere facturas existentes para mostrar informacion.
 */
void facturaMostrarListadoAdmin(const char *ruta)
{
    Factura *facturas = NULL;
    int cantidad = 0;

    if (!facturaCargarTodas(ruta, &facturas, &cantidad) || cantidad == 0)
    {
        printf(MSG_WARNING "No hay facturas registradas.\n" RESET);
        return;
    }

    printf("\n" MENU_BORDER "================================================================================\n" RESET);
    printf(MENU_TITLE "                              LISTADO DE FACTURAS\n" RESET);
    printf(MENU_BORDER "================================================================================\n" RESET);
    printf("%-5s %-20s %-12s %-20s %-12s\n", "ID", "Evento", "Fecha compra", "Cliente", "Subtotal");
    printf(MENU_BORDER "--------------------------------------------------------------------------------\n" RESET);

    for (int i = 0; i < cantidad; i++)
    {
        printf("%-5d %-20.20s %-12s %-20.20s %-12.2f\n",
               facturas[i].id,
               facturas[i].evento,
               facturas[i].fechaCompra,
               facturas[i].cliente,
               facturas[i].subtotal);
    }

    facturaLiberarLista(facturas);
}

/*
 * Objetivo: Calcular el total recaudado para un evento especifico.
 * Entradas: ruta del archivo de facturas y nombreEvento a filtrar.
 * Salidas: suma total de facturas asociadas al evento.
 * Restricciones: retorna 0.0 si no hay datos o si nombreEvento es NULL.
 */
float facturaTotalPorEvento(const char *ruta, const char *nombreEvento)
{
    Factura *facturas = NULL;
    int cantidad = 0;
    float total = 0.0f;

    if (nombreEvento == NULL)
    {
        return 0.0f;
    }

    if (!facturaCargarTodas(ruta, &facturas, &cantidad))
    {
        return 0.0f;
    }

    for (int i = 0; i < cantidad; i++)
    {
        if (strcmp(facturas[i].evento, nombreEvento) == 0)
        {
            total += facturas[i].total;
        }
    }

    facturaLiberarLista(facturas);
    return total;
}
