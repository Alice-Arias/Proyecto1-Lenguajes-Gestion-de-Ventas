#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../security/hash.h"
#include "admin.h"
#include "../include/sitio.h"
#include "../include/colors.h"
#include "../include/evento.h"
#include "../include/sector.h"
#include "../include/asiento.h"

Usuario *listaUsuarios = NULL;
int cantidadUsuarios = 0;

void cargarUsuarios()
{

    FILE *archivo = fopen(ARCHIVO_USUARIOS, "r");

    if (!archivo)
    {
        printf(MSG_INFO "No existe archivo, creando...\n" RESET);
        archivo = fopen(ARCHIVO_USUARIOS, "w");
        fclose(archivo);
        return;
    }

    Usuario temp;

    while (fscanf(archivo, " %49[^,],%lu", temp.usuario, &temp.hash) == 2)
    {

        cantidadUsuarios++;

        Usuario *tempPtr = realloc(listaUsuarios, cantidadUsuarios * sizeof(Usuario));
        if (tempPtr == NULL)
        {
            printf(MSG_ERROR "Error en memoria\n" RESET);
            return;
        }

        listaUsuarios = tempPtr;
        listaUsuarios[cantidadUsuarios - 1] = temp;
    }

    fclose(archivo);
}

int IniciarSesionAdmin()
{

    char usuarioIngresado[50];
    char contrasenaIngresada[50];
    
    printf("\n" MENU_BORDER "====================================\n" RESET);
    printf(MENU_INPUT "Usuario: " RESET);
    scanf("%49s", usuarioIngresado);
    printf(MENU_INPUT "Contrasena: " RESET);
    scanf("%49s", contrasenaIngresada);

    unsigned long hashIngresado = hashContrasena(contrasenaIngresada);
    for (int i = 0; i < cantidadUsuarios; i++)
    {
        if (strcmp(usuarioIngresado, listaUsuarios[i].usuario) == 0)
        {
            if (hashIngresado == listaUsuarios[i].hash)
            {
                printf(MSG_SUCCESS "Inicio de sesion exitoso\n" RESET);
                return 1;
            }
            else
            {
                printf(MSG_ERROR "Contrasena incorrecta111\n" RESET);
                return 0;
            }
        }
    }

    printf(MSG_ERROR "Usuario no encontrado\n" RESET);
    return 0;
}

void menuEventos()
{
    int opcion;

    do
    {
        printf("\n" MENU_BORDER "====================================\n" RESET);
        printf(MENU_TITLE "      GESTION DE EVENTOS\n" RESET);
        printf(MENU_BORDER "====================================\n" RESET);

        printf(MENU_OPTION "1. Crear evento\n" RESET);
        printf(MENU_OPTION "2. Mostrar eventos\n" RESET);
        printf(MENU_OPTION "3. Volver\n" RESET);

        printf(MENU_BORDER "====================================\n" RESET);

        printf(MENU_INPUT "Seleccione: " RESET);
        scanf("%d", &opcion);

        switch (opcion)
        {

        case 1:
            pedirDatosYCrearEvento();
            break;
        case 2:
            mostrarEventos();
            break;

        case 3:
            printf(MSG_INFO "Volviendo...\n" RESET);
            break;

        default:
            printf(MSG_ERROR "Opcion invalida\n" RESET);
        }

    } while (opcion != 3);
}

void menuSitios()
{
    int opcion;
    char ruta[200];

    do
    {
        printf("\n" MENU_BORDER "====================================\n" RESET);
        printf(MENU_TITLE "      GESTION DE SITIOS\n" RESET);
        printf(MENU_BORDER "====================================\n" RESET);

        printf(MENU_OPTION "1. Cargar sitios desde archivo\n" RESET);
        printf(MENU_OPTION "2. Agregar sitio manualmente\n" RESET);
        printf(MENU_OPTION "3. Mostrar sitios\n" RESET);
        printf(MENU_OPTION "4. Guardar sitios\n" RESET);
        printf(MENU_OPTION "5. Volver\n" RESET);

        printf(MENU_BORDER "====================================\n" RESET);

        printf(MENU_INPUT "Seleccione: " RESET);
        scanf("%d", &opcion);

        switch (opcion)
        {

        case 1:
            printf("Ingrese ruta del archivo: ");
            scanf("%s", ruta);
            cargarSitiosDesdeArchivo(ruta);
            break;

        case 2:
        {
            char nombre[50], ubicacion[150], web[100];

            printf("Nombre: ");
            scanf(" %[^\n]", nombre);

            printf("Ubicacion: ");
            scanf(" %[^\n]", ubicacion);

            printf("Web (opcional): ");
            scanf(" %[^\n]", web);

            if (strlen(nombre) == 0 || strlen(ubicacion) == 0)
            {
                printf(MSG_ERROR "Datos invalidos.\n" RESET);
                break;
            }

            agregarSitio(nombre, ubicacion, web);
            break;
        }

        case 3:
            mostrarSitios();
            break;

        case 4:
            guardarSitiosEnArchivo("data/sitios.txt");
            break;

        case 5:
            printf("Volviendo...\n");
            break;

        default:
            printf("Opcion invalida\n");
        }

    } while (opcion != 5);
}

void menuSectores()
{
    int opcion;
    int indiceSitio;
    char nombre[50];
    char inicial;
    int cantidad;

    do
    {
        printf("\n" MENU_BORDER "====================================\n" RESET);
        printf(MENU_TITLE "      GESTION DE ESPACIOS\n" RESET);
        printf(MENU_BORDER "====================================\n" RESET);

        printf(MENU_OPTION "1. Ver sectores de un sitio\n" RESET);
        printf(MENU_OPTION "2. Agregar sector a un sitio\n" RESET);
        printf(MENU_OPTION "3. Resetear sectores de un sitio\n" RESET);
        printf(MENU_OPTION "4. Volver\n" RESET);

        printf(MENU_BORDER "====================================\n" RESET);

        printf(MENU_INPUT "Seleccione: " RESET);
        scanf("%d", &opcion);

        switch (opcion)
        {

        case 1:
            mostrarSitios();
            printf("Seleccione sitio: ");
            scanf("%d", &indiceSitio);

            if (indiceSitio < 1 || indiceSitio > cantidadSitios)
            {
                printf(MSG_ERROR "Sitio invalido\n" RESET);
                break;
            }

            mostrarSectoresDeSitio(&listaSitios[indiceSitio - 1]);
            break;

        case 2:
            mostrarSitios();
            printf("Seleccione sitio: ");
            scanf("%d", &indiceSitio);

            if (indiceSitio < 1 || indiceSitio > cantidadSitios)
            {
                printf(MSG_ERROR "Sitio invalido\n" RESET);
                break;
            }

            printf("Nombre del sector: ");
            scanf(" %[^\n]", nombre);

            printf("Inicial (letra): ");
            scanf(" %c", &inicial);

            printf("Cantidad de asientos: ");
            scanf("%d", &cantidad);

            agregarSectorASitio(
                &listaSitios[indiceSitio - 1],
                nombre,
                inicial,
                cantidad);
            break;

        case 3:
            mostrarSitios();
            printf("Seleccione sitio: ");
            scanf("%d", &indiceSitio);

            if (indiceSitio < 1 || indiceSitio > cantidadSitios)
            {
                printf(MSG_ERROR "Sitio invalido\n" RESET);
                break;
            }

            resetSectoresDeSitio(&listaSitios[indiceSitio - 1]);
            break;

        case 4:
            printf("Volviendo...\n");
            break;

        default:
            printf(MSG_ERROR "Opcion invalida\n" RESET);
        }

    } while (opcion != 4);
}

// ==================== ESTADÍSTICAS ====================

static void mostrarTopMeses()
{
    // Contar eventos por mes-año
    struct MesCount
    {
        char mesAnio[10];
        int count;
    } meses[100];
    int mesesCount = 0;

    for (int i = 0; i < totalEventos; i++)
    {
        int d, m, y;
        sscanf(eventos[i].fecha, "%d/%d/%d", &d, &m, &y);
        char key[10];
        sprintf(key, "%d/%d", m, y); // formato "mes/año"

        int idx = -1;
        for (int j = 0; j < mesesCount; j++)
        {
            if (strcmp(meses[j].mesAnio, key) == 0)
            {
                idx = j;
                break;
            }
        }
        if (idx == -1)
        {
            strcpy(meses[mesesCount].mesAnio, key);
            meses[mesesCount].count = 1;
            mesesCount++;
        }
        else
        {
            meses[idx].count++;
        }
    }

    // Ordenar descendente por cantidad
    for (int i = 0; i < mesesCount - 1; i++)
    {
        for (int j = i + 1; j < mesesCount; j++)
        {
            if (meses[i].count < meses[j].count)
            {
                struct MesCount tmp = meses[i];
                meses[i] = meses[j];
                meses[j] = tmp;
            }
        }
    }

    // Mostrar top 3
    printf("\n" MENU_BORDER "========================================\n" RESET);
    printf(MENU_TITLE "   TOP 3 MESES CON MAS EVENTOS\n" RESET);
    printf(MENU_BORDER "========================================\n" RESET);
    int top = (mesesCount < 3) ? mesesCount : 3;
    for (int i = 0; i < top; i++)
    {
        printf("%d. %-10s %d eventos\n", i + 1, meses[i].mesAnio, meses[i].count);
    }
    if (top == 0)
    {
        printf(MSG_INFO "No hay eventos registrados.\n" RESET);
    }
}

static void mostrarTopProductoras()
{
    FILE *f = fopen("data/facturas.txt", "r");
    if (!f)
    {
        printf(MSG_WARNING "No hay facturas registradas.\n" RESET);
        return;
    }

    struct ProdTotal
    {
        char nombre[MAX_PRODUCTORA];
        float total;
    } prod[100];
    int prodCount = 0;

    char line[256];
    char currentProd[100] = "";
    float currentTotal = 0.0;
    int inFactura = 0;

    while (fgets(line, sizeof(line), f))
    {
        line[strcspn(line, "\n")] = '\0';
        if (strstr(line, "========== FACTURA ==========") != NULL)
        {
            currentProd[0] = '\0';
            currentTotal = 0.0;
            inFactura = 1;
        }
        else if (inFactura && strstr(line, "Productora:") != NULL)
        {
            sscanf(line, "Productora: %[^\n]", currentProd);
        }
        else if (inFactura && strstr(line, "TOTAL:") != NULL)
        {
            sscanf(line, "TOTAL: %f", &currentTotal);
            if (currentProd[0] != '\0')
            {
                int idx = -1;
                for (int i = 0; i < prodCount; i++)
                {
                    if (strcmp(prod[i].nombre, currentProd) == 0)
                    {
                        idx = i;
                        break;
                    }
                }
                if (idx == -1)
                {
                    idx = prodCount;
                    strcpy(prod[idx].nombre, currentProd);
                    prod[idx].total = 0.0;
                    prodCount++;
                }
                prod[idx].total += currentTotal;
            }
            inFactura = 0;
        }
    }
    fclose(f);

    // Ordenar descendente por total
    for (int i = 0; i < prodCount - 1; i++)
    {
        for (int j = i + 1; j < prodCount; j++)
        {
            if (prod[i].total < prod[j].total)
            {
                struct ProdTotal tmp = prod[i];
                prod[i] = prod[j];
                prod[j] = tmp;
            }
        }
    }

    printf("\n" MENU_BORDER "========================================\n" RESET);
    printf(MENU_TITLE "   TOP 3 PRODUCTORAS CON MAS RECAUDACION\n" RESET);
    printf(MENU_BORDER "========================================\n" RESET);
    int top = (prodCount < 3) ? prodCount : 3;
    for (int i = 0; i < top; i++)
    {
        printf("%d. %-30s %.2f\n", i + 1, prod[i].nombre, prod[i].total);
    }
    if (top == 0)
    {
        printf(MSG_INFO "No hay facturas registradas.\n" RESET);
    }
}

static void mostrarSitiosConEventosYRecaudacion()
{
    // 1. Contar eventos por sitio
    struct SitioData
    {
        char nombre[MAX_NOMBRE];
        int eventos;
        float recaudacion;
    } sitios[100];
    int sitiosCount = 0;

    for (int i = 0; i < totalEventos; i++)
    {
        char *sitioNombre = eventos[i].sitio->nombre;
        int idx = -1;
        for (int j = 0; j < sitiosCount; j++)
        {
            if (strcmp(sitios[j].nombre, sitioNombre) == 0)
            {
                idx = j;
                break;
            }
        }
        if (idx == -1)
        {
            idx = sitiosCount;
            strcpy(sitios[idx].nombre, sitioNombre);
            sitios[idx].eventos = 0;
            sitios[idx].recaudacion = 0.0;
            sitiosCount++;
        }
        sitios[idx].eventos++;
    }

    // 2. Leer facturas para sumar recaudación por sitio
    FILE *f = fopen("data/facturas.txt", "r");
    if (f)
    {
        char line[256];
        char currentSitio[100] = "";
        float currentTotal = 0.0;
        int inFactura = 0;
        while (fgets(line, sizeof(line), f))
        {
            line[strcspn(line, "\n")] = '\0';
            if (strstr(line, "========== FACTURA ==========") != NULL)
            {
                currentSitio[0] = '\0';
                currentTotal = 0.0;
                inFactura = 1;
            }
            else if (inFactura && strstr(line, "Sitio:") != NULL)
            {
                sscanf(line, "Sitio: %[^\n]", currentSitio);
            }
            else if (inFactura && strstr(line, "TOTAL:") != NULL)
            {
                sscanf(line, "TOTAL: %f", &currentTotal);
                if (currentSitio[0] != '\0')
                {
                    for (int i = 0; i < sitiosCount; i++)
                    {
                        if (strcmp(sitios[i].nombre, currentSitio) == 0)
                        {
                            sitios[i].recaudacion += currentTotal;
                            break;
                        }
                    }
                }
                inFactura = 0;
            }
        }
        fclose(f);
    }

    // 3. Ordenar por recaudación descendente
    for (int i = 0; i < sitiosCount - 1; i++)
    {
        for (int j = i + 1; j < sitiosCount; j++)
        {
            if (sitios[i].recaudacion < sitios[j].recaudacion)
            {
                struct SitioData tmp = sitios[i];
                sitios[i] = sitios[j];
                sitios[j] = tmp;
            }
        }
    }

    // 4. Mostrar
    printf("\n" MENU_BORDER "========================================\n" RESET);
    printf(MENU_TITLE "    SITIOS CON EVENTOS Y RECAUDACION\n" RESET);
    printf(MENU_BORDER "========================================\n" RESET);
    printf("%-20s %-10s %-15s\n", "Sitio", "Eventos", "Recaudacion");
    printf(MENU_BORDER "----------------------------------------\n" RESET);
    for (int i = 0; i < sitiosCount; i++)
    {
        printf("%-20s %-10d %-15.2f\n", sitios[i].nombre, sitios[i].eventos, sitios[i].recaudacion);
    }
    if (sitiosCount == 0)
    {
        printf(MSG_INFO "No hay eventos registrados.\n" RESET);
    }
}

// Función que agrupa las tres estadísticas
void mostrarEstadisticas()
{
    mostrarTopMeses();
    mostrarTopProductoras();
    mostrarSitiosConEventosYRecaudacion();
}

void menuAdmin()
{
    int opcion;

    do
    {
        printf("\n" MENU_BORDER "=========================================\n" RESET);
        printf(MENU_TITLE "        MENU ADMINISTRADOR\n" RESET);
        printf(MENU_BORDER "=========================================\n" RESET);

        printf(MENU_OPTION "1. Gestion de Sitios de Eventos\n" RESET);
        printf(MENU_OPTION "2. Gestion de espacios sitio de eventos\n" RESET);
        printf(MENU_OPTION "3. Gestion de Eventos\n" RESET);
        printf(MENU_OPTION "4. Estado de evento\n" RESET);
        printf(MENU_OPTION "5. Lista de facturas\n" RESET);
        printf(MENU_OPTION "6. Estadisticas\n" RESET);
        printf(MENU_OPTION "7. Volver\n" RESET);

        printf(MENU_BORDER "=========================================\n" RESET);

        printf(MENU_INPUT "Seleccione: " RESET);
        scanf("%d", &opcion);

        switch (opcion)
        {
        case 1:
            menuSitios();
            break;

        case 2:
            menuSectores();
            break;

        case 3:
            menuEventos();
            break;

        case 4:
            mostrarEstadoEvento();
            break;

        case 5:
            printf("Aqui va lista de facturas...\n");
            break;

        case 6:
            mostrarEstadisticas();
            break;
        case 7:
            printf("Volviendo...\n");
            break;

        default:
            printf("Opcion invalida\n");
        }

    } while (opcion != 7);
}

void liberarUsuarios()
{
    free(listaUsuarios);
    listaUsuarios = NULL;
    cantidadUsuarios = 0;
}