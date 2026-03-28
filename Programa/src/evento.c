#include "../include/evento.h"
#include "../include/sitio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/colors.h"
#include "../include/factura.h"

Evento *eventos = NULL;
int totalEventos = 0;

static void limpiarBufferEntradaEvento(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

static int leerEnteroEvento(const char *prompt, int *valor)
{
    if (prompt != NULL)
    {
        printf("%s", prompt);
    }
    if (scanf("%d", valor) != 1)
    {
        limpiarBufferEntradaEvento();
        return 0;
    }
    return 1;
}

static int leerFloatEvento(const char *prompt, float *valor)
{
    if (prompt != NULL)
    {
        printf("%s", prompt);
    }
    if (scanf("%f", valor) != 1)
    {
        limpiarBufferEntradaEvento();
        return 0;
    }
    return 1;
}

static int esBisiestoEvento(int anio)
{
    return (anio % 400 == 0) || (anio % 4 == 0 && anio % 100 != 0);
}

static void setMensajeErrorEvento(char *mensaje, size_t tam, const char *texto)
{
    if (mensaje != NULL && tam > 0)
    {
        snprintf(mensaje, tam, "%s", texto);
    }
}

static int validarFechaEventoConDetalle(const char *fecha, char *mensaje, size_t tam)
{
    int d, m, a;
    int diasMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (sscanf(fecha, "%d/%d/%d", &d, &m, &a) != 3)
    {
        setMensajeErrorEvento(mensaje, tam, "formato invalido; use dd/mm/aaaa (ejemplo: 28/03/2026)");
        return 0;
    }

    if (a < 1900)
    {
        setMensajeErrorEvento(mensaje, tam, "anio invalido; el anio minimo permitido es 1900");
        return 0;
    }

    if (a > 9999)
    {
        setMensajeErrorEvento(mensaje, tam, "anio invalido; el anio maximo permitido es 9999");
        return 0;
    }

    if (m < 1 || m > 12)
    {
        setMensajeErrorEvento(mensaje, tam, "mes invalido; debe estar entre 1 y 12");
        return 0;
    }

    if (m == 2 && esBisiestoEvento(a))
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

static int esFechaValidaEvento(const char *fecha)
{
    return validarFechaEventoConDetalle(fecha, NULL, 0);
}

void inicializarEventos()
{
    if (eventos != NULL)
    {
        liberarEventos();
    }
    eventos = NULL;
    totalEventos = 0;
}

void crearEvento(const char *nombre, const char *productora, const char *fecha,
                 SitioEvento *sitio, float *preciosPorSector)
{
    if (sitio == NULL)
    {
        printf(MSG_ERROR "Error: Sitio no válido.\n" RESET);
        return;
    }

    // Verificar que no exista un evento con el mismo nombre
    for (int i = 0; i < totalEventos; i++)
    {
        if (strcmp(eventos[i].nombre, nombre) == 0)
        {
            printf(MSG_ERROR "Error: Ya existe un evento con nombre '%s'.\n" RESET, nombre);
            return;
        }
    }

    // Ampliar arreglo de eventos
    Evento *temp = realloc(eventos, (totalEventos + 1) * sizeof(Evento));
    if (temp == NULL)
    {
        printf(MSG_ERROR "Error de memoria al crear evento.\n" RESET);
        return;
    }
    eventos = temp;

    Evento *nuevo = &eventos[totalEventos];
    strncpy(nuevo->nombre, nombre, MAX_NOMBRE_EVENTO - 1);
    nuevo->nombre[MAX_NOMBRE_EVENTO - 1] = '\0';
    strncpy(nuevo->productora, productora, MAX_PRODUCTORA - 1);
    nuevo->productora[MAX_PRODUCTORA - 1] = '\0';
    strncpy(nuevo->fecha, fecha, MAX_FECHA - 1);
    nuevo->fecha[MAX_FECHA - 1] = '\0';
    nuevo->sitio = sitio;

    // Reservar memoria para los precios (uno por sector del sitio) ----
    int numSectores = sitio->totalSectores;
    nuevo->precios = (PrecioSector *)malloc(numSectores * sizeof(PrecioSector));
    if (nuevo->precios == NULL)
    {
        printf(MSG_ERROR "Error de memoria para precios.\n" RESET);

        return;
    }
    for (int i = 0; i < numSectores; i++)
    {
        strncpy(nuevo->precios[i].nombreSector, sitio->sectores[i].nombre, MAX_SECTOR_NOMBRE - 1);
        nuevo->precios[i].nombreSector[MAX_SECTOR_NOMBRE - 1] = '\0';
        nuevo->precios[i].precio = preciosPorSector[i];
    }

    // Crear matriz de disponibilidad: para cada sector, un arreglo de bytes (1 = disponible)
    nuevo->disponibilidad = (int **)malloc(numSectores * sizeof(int *));
    if (nuevo->disponibilidad == NULL)
    {
        free(nuevo->precios);
        printf(MSG_ERROR "Error de memoria para disponibilidad.\n" RESET);
        return;
    }
    for (int i = 0; i < numSectores; i++)
    {
        int asientosPorSector = sitio->sectores[i].cantidadEspacios;
        nuevo->disponibilidad[i] = (int *)calloc(asientosPorSector, sizeof(int));
        if (nuevo->disponibilidad[i] == NULL)
        {
            // Limpiar lo ya asignado
            for (int j = 0; j < i; j++)
                free(nuevo->disponibilidad[j]);
            free(nuevo->disponibilidad);
            free(nuevo->precios);
            printf(MSG_ERROR "Error de memoria para disponibilidad sector %d.\n" RESET, i);
            return;
        }
        // Inicializar todos a 1 (disponible)
        for (int j = 0; j < asientosPorSector; j++)
        {
            nuevo->disponibilidad[i][j] = 1;
        }
    }

    totalEventos++;
    printf(MSG_INFO "Evento '%s' creado correctamente en sitio '%s'.\n" RESET, nombre, sitio->nombre);
}

void pedirDatosYCrearEvento()
{
    char nombre[50], productora[50], fecha[20];
    char detalleErrorFecha[160];
    int indiceSitio;

    printf(MENU_INPUT "Nombre del evento: " RESET);
    scanf(" %[^\n]", nombre);

    printf(MENU_INPUT "Productora: " RESET);
    scanf(" %[^\n]", productora);

    printf(MENU_INPUT "Fecha: " RESET);
    scanf(" %[^\n]", fecha);

    if (!validarFechaEventoConDetalle(fecha, detalleErrorFecha, sizeof(detalleErrorFecha)))
    {
        printf(MSG_ERROR "Fecha invalida: %s.\n" RESET, detalleErrorFecha);
        return;
    }

    mostrarSitios();
    if (!leerEnteroEvento(MENU_INPUT "Seleccione sitio: " RESET, &indiceSitio))
    {
        printf(MSG_ERROR "Entrada invalida. Debe ingresar un numero entero para el sitio.\n" RESET);
        return;
    }

    if (indiceSitio < 1 || indiceSitio > cantidadSitios)
    {
        printf(MSG_ERROR "Sitio invalido. Debe seleccionar un valor entre 1 y %d.\n" RESET, cantidadSitios);
        return;
    }

    SitioEvento *sitio = &listaSitios[indiceSitio - 1];

    mostrarSectoresDeSitio(sitio);

    int numSectores = sitio->totalSectores;

    if (numSectores == 0)
    {
        printf(MSG_ERROR "El sitio no tiene sectores.\n" RESET);
        return;
    }

    float *precios = malloc(numSectores * sizeof(float));
    if (!precios)
    {
        printf(MSG_ERROR "Error de memoria\n" RESET);
        return;
    }

    for (int i = 0; i < numSectores; i++)
    {
        char prompt[180];
        snprintf(prompt, sizeof(prompt), MENU_INPUT "Precio para sector '%s': " RESET, sitio->sectores[i].nombre);
        if (!leerFloatEvento(prompt, &precios[i]))
        {
            printf(MSG_ERROR "Precio invalido. Debe ingresar un numero (ejemplo: 15000 o 15000.50).\n" RESET);
            free(precios);
            return;
        }
    }

    crearEvento(nombre, productora, fecha, sitio, precios);

    free(precios);
}
void liberarEvento(Evento *evento)
{
    if (evento == NULL)
        return;
    if (evento->precios)
        free(evento->precios);
    if (evento->disponibilidad)
    {
        for (int i = 0; i < evento->sitio->totalSectores; i++)
        {
            free(evento->disponibilidad[i]);
        }
        free(evento->disponibilidad);
    }
}

void liberarEventos()
{
    for (int i = 0; i < totalEventos; i++)
    {
        liberarEvento(&eventos[i]);
    }
    free(eventos);
    eventos = NULL;
    totalEventos = 0;
}

void guardarEventosEnArchivo(const char *ruta)
{
    FILE *f = fopen(ruta, "w");
    if (!f)
    {
        printf(MSG_ERROR "Error al abrir archivo %s para guardar eventos.\n" RESET, ruta);
        return;
    }

    for (int i = 0; i < totalEventos; i++)
    {
        Evento *e = &eventos[i];
        // Formato: nombre,productora,fecha,sitio_nombre,precio1,precio2,...
        fprintf(f, "%s,%s,%s,%s", e->nombre, e->productora, e->fecha, e->sitio->nombre);
        for (int s = 0; s < e->sitio->totalSectores; s++)
        {
            fprintf(f, ",%.2f", e->precios[s].precio);
        }
        fprintf(f, "\n");
    }
    fclose(f);
    printf(MSG_INFO "Eventos guardados en %s.\n" RESET, ruta);
}

void cargarEventosDesdeArchivo(const char *ruta)
{
    FILE *f = fopen(ruta, "r");
    if (!f)
    {
        // No existe archivo, no hay error
        return;
    }

    // Leer línea por línea
    char linea[1024];
    while (fgets(linea, sizeof(linea), f))
    {
        linea[strcspn(linea, "\n")] = '\0';
        char *token = strtok(linea, ",");
        if (!token)
            continue;
        char nombre[MAX_NOMBRE_EVENTO];
        strncpy(nombre, token, MAX_NOMBRE_EVENTO - 1);
        nombre[MAX_NOMBRE_EVENTO - 1] = '\0';

        char productora[MAX_PRODUCTORA];
        token = strtok(NULL, ",");
        if (!token)
            continue;
        strncpy(productora, token, MAX_PRODUCTORA - 1);
        productora[MAX_PRODUCTORA - 1] = '\0';

        char fecha[MAX_FECHA];
        token = strtok(NULL, ",");
        if (!token)
            continue;
        strncpy(fecha, token, MAX_FECHA - 1);
        fecha[MAX_FECHA - 1] = '\0';

        char nombreSitio[MAX_NOMBRE];
        token = strtok(NULL, ",");
        if (!token)
            continue;
        strncpy(nombreSitio, token, MAX_NOMBRE - 1);
        nombreSitio[MAX_NOMBRE - 1] = '\0';

        // Buscar el sitio por nombre
        SitioEvento *sitio = NULL;
        for (int i = 0; i < cantidadSitios; i++)
        {
            if (strcmp(listaSitios[i].nombre, nombreSitio) == 0)
            {
                sitio = &listaSitios[i];
                break;
            }
        }
        if (sitio == NULL)
        {
            printf(MSG_WARNING "Advertencia: sitio '%s' no encontrado para evento '%s'\n" RESET, nombreSitio, nombre);
            continue;
        }

        int numSectores = sitio->totalSectores;
        float *precios = (float *)malloc(numSectores * sizeof(float));
        if (precios == NULL)
        {
            printf(MSG_ERROR "Error de memoria al cargar precios.\n" RESET);
            fclose(f);
            return;
        }
        for (int i = 0; i < numSectores; i++)
        {
            token = strtok(NULL, ",");
            if (token)
            {
                precios[i] = atof(token);
            }
            else
            {
                precios[i] = 0.0;
            }
        }
        crearEvento(nombre, productora, fecha, sitio, precios);
        free(precios);
    }
    fclose(f);
}

void mostrarEventos()
{
    printf("\n" MENU_BORDER "====================================\n" RESET);
    printf(COLOR_EVENTO BOLD "         LISTA DE EVENTOS\n" RESET);
    printf(MENU_BORDER "====================================\n" RESET);

    for (int i = 0; i < totalEventos; i++)
    {
        Evento *e = &eventos[i];

        printf(COLOR_EVENTO "%d. %s" RESET, i + 1, e->nombre);
        printf(" | ");
        printf(COLOR_USUARIO "%s" RESET, e->productora);
        printf(" | ");
        printf(MSG_INFO "%s" RESET, e->fecha);
        printf(" | Sitio: ");
        printf(COLOR_SITIO "%s\n" RESET, e->sitio->nombre);
    }

    printf("\n");
}

int verificarDisponibilidad(Evento *evento, int sectorIdx, int asientoIdx)
{
    if (evento == NULL || sectorIdx < 0 || sectorIdx >= evento->sitio->totalSectores)
        return 0;
    if (asientoIdx < 0 || asientoIdx >= evento->sitio->sectores[sectorIdx].cantidadEspacios)
        return 0;
    return evento->disponibilidad[sectorIdx][asientoIdx];
}

void marcarVendido(Evento *evento, int sectorIdx, int asientoIdx)
{
    if (evento == NULL)
        return;
    if (sectorIdx >= 0 && sectorIdx < evento->sitio->totalSectores &&
        asientoIdx >= 0 && asientoIdx < evento->sitio->sectores[sectorIdx].cantidadEspacios)
    {
        evento->disponibilidad[sectorIdx][asientoIdx] = 0;
        printf(MSG_SUCCESS "Asiento vendido correctamente.\n" RESET);
    }
}

float obtenerPrecioAsiento(Evento *evento, int sectorIdx, int asientoIdx)
{
    if (evento == NULL || sectorIdx < 0 || sectorIdx >= evento->sitio->totalSectores)
        return 0.0;
    return evento->precios[sectorIdx].precio;
}

float calcularRecaudadoPorEvento(const char *nombreEvento)
{
    return facturaTotalPorEvento(ARCHIVO_FACTURAS, nombreEvento);
}

void mostrarEstadoEvento()
{
    if (totalEventos == 0)
    {
        printf(MSG_ERROR "No hay eventos creados.\n" RESET);
        return;
    }

    mostrarEventos();

    int opcion;
    if (!leerEnteroEvento(MENU_INPUT "Seleccione un evento: " RESET, &opcion))
    {
        printf(MSG_ERROR "Entrada invalida\n" RESET);
        return;
    }

    if (opcion < 1 || opcion > totalEventos)
    {
        printf(MSG_ERROR "Evento inválido.\n" RESET);
        return;
    }

    Evento *e = &eventos[opcion - 1];

    printf("\n" MENU_BORDER "=====================================\n" RESET);
    printf(COLOR_EVENTO BOLD UNDERLINE "        ESTADO DEL EVENTO\n" RESET);
    printf(MENU_BORDER "=====================================\n" RESET);

    printf(COLOR_EVENTO BOLD "Nombre: " RESET "%s\n", e->nombre);
    printf(COLOR_EVENTO BOLD "Productora: " RESET "%s\n", e->productora);
    printf(COLOR_EVENTO BOLD "Fecha evento: " RESET "%s\n", e->fecha);
    printf(COLOR_EVENTO BOLD "Sitio: " RESET "%s\n", e->sitio->nombre);

    printf(MENU_BORDER "-------------------------------------\n" RESET);

    float totalRecaudado = calcularRecaudadoPorEvento(e->nombre);

    float totalEvento = 0.0;

    for (int s = 0; s < e->sitio->totalSectores; s++)
    {
        printf("\n" COLOR_SITIO BOLD " Sector: %s\n" RESET, e->sitio->sectores[s].nombre);
        printf(COLOR_SITIO " Precio por asiento: %.2f\n" RESET, e->precios[s].precio);

        int totalAsientos = e->sitio->sectores[s].cantidadEspacios;
        char inicial = e->sitio->sectores[s].nombre[0];

        printf("\n" COLOR_ASIENTO BOLD " Asientos:\n" RESET);

        float recaudadoSector = 0.0;

        for (int a = 0; a < totalAsientos; a++)
        {
            printf("%c%d - ", inicial, a + 1);

            if (e->disponibilidad[s][a] == 0)
            {
                printf(RED "VENDIDO\n" RESET);

                recaudadoSector += e->precios[s].precio;
            }
            else
            {
                printf(GREEN "DISPONIBLE\n" RESET);
            }
        }

        printf(MSG_INFO "Recaudado sector: %.2f\n" RESET, recaudadoSector);

        totalEvento += recaudadoSector;

        printf(MENU_BORDER "-------------------------------------\n" RESET);
    }

    printf("\n" MSG_SUCCESS BOLD " TOTAL RECAUDADO : %.2f\n" RESET, totalRecaudado);

    printf(MENU_BORDER "=====================================\n" RESET);
}