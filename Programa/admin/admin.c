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
#include "../include/factura.h"

Usuario *listaUsuarios = NULL;
int cantidadUsuarios = 0;

/*
 * Objetivo: Limpiar el buffer de entrada estandar tras lecturas invalidas.
 * Entradas: No recibe parametros.
 * Salidas: consume caracteres hasta fin de linea o EOF.
 * Restricciones: usar despues de scanf fallido.
 */
static void limpiarBufferEntradaAdmin(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
    }
}

/*
 * Objetivo: Leer un entero para menus administrativos con validacion.
 * Entradas: prompt opcional y puntero destino.
 * Salidas: retorna 1 si lectura correcta, 0 si invalida.
 * Restricciones: valor no debe ser NULL.
 */
static int leerEnteroAdmin(const char *prompt, int *valor)
{
    if (prompt != NULL)
    {
        printf("%s", prompt);
    }
    if (scanf("%d", valor) != 1)
    {
        limpiarBufferEntradaAdmin();
        return 0;
    }
    return 1;
}

/*
 * Objetivo: Cargar usuarios administradores desde archivo de credenciales.
 * Entradas: No recibe parametros; usa ARCHIVO_USUARIOS.
 * Salidas: llena listaUsuarios y cantidadUsuarios en memoria.
 * Restricciones: formato esperado por linea: usuario,hash.
 */
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

/*
 * Objetivo: Validar credenciales y autorizar acceso al menu administrador.
 * Entradas: usuario y contrasena leidos desde consola.
 * Salidas: retorna 1 si credenciales validas, 0 en caso contrario.
 * Restricciones: depende de que cargarUsuarios() haya inicializado la lista.
 */
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
                printf(MSG_ERROR "Contrasena incorrecta\n" RESET);
                return 0;
            }
        }
    }

    printf(MSG_ERROR "Usuario no encontrado\n" RESET);
    return 0;
}

/*
 * Objetivo: Administrar operaciones relacionadas con eventos.
 * Entradas: seleccion de opcion por consola.
 * Salidas: ejecuta crear/mostrar eventos o retorna al menu anterior.
 * Restricciones: bucle finaliza cuando opcion es 3.
 */
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

        if (!leerEnteroAdmin(MENU_INPUT "Seleccione: " RESET, &opcion))
        {
            printf(MSG_ERROR "Entrada invalida\n" RESET);
            continue;
        }

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

/*
 * Objetivo: Administrar carga, alta, visualizacion y guardado de sitios.
 * Entradas: opcion del usuario y, segun caso, datos de sitio/ruta.
 * Salidas: invoca funciones de mantenimiento de sitios.
 * Restricciones: bucle finaliza cuando opcion es 5.
 */
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

        if (!leerEnteroAdmin(MENU_INPUT "Seleccione: " RESET, &opcion))
        {
            printf(MSG_ERROR "Entrada invalida\n" RESET);
            continue;
        }

        switch (opcion)
        {

        case 1:
            printf(MENU_INPUT "Ingrese ruta del archivo: " RESET);
            scanf("%199s", ruta);
            cargarSitiosDesdeArchivo(ruta);
            break;

        case 2:
        {
            char nombre[50], ubicacion[150], web[100];

            printf(MENU_INPUT "Nombre: " RESET);
            scanf(" %[^\n]", nombre);

            printf(MENU_INPUT "Ubicacion: " RESET);
            scanf(" %[^\n]", ubicacion);

            printf(MENU_INPUT "Web (opcional): " RESET);
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
            printf(MSG_INFO "Volviendo...\n" RESET);
            break;

        default:
            printf(MSG_ERROR "Opcion invalida\n" RESET);
        }

    } while (opcion != 5);
}

/*
 * Objetivo: Administrar sectores de los sitios existentes.
 * Entradas: opcion del usuario, sitio objetivo y datos de sector.
 * Salidas: permite listar/agregar/resetear sectores.
 * Restricciones: valida que el sitio seleccionado este en rango.
 */
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

        if (!leerEnteroAdmin(MENU_INPUT "Seleccione: " RESET, &opcion))
        {
            printf(MSG_ERROR "Entrada invalida\n" RESET);
            continue;
        }

        switch (opcion)
        {

        case 1:
            mostrarSitios();
            printf(MENU_INPUT "Seleccione sitio: " RESET);
            if (!leerEnteroAdmin(NULL, &indiceSitio))
            {
                printf(MSG_ERROR "Entrada invalida\n" RESET);
                break;
            }

            if (indiceSitio < 1 || indiceSitio > cantidadSitios)
            {
                printf(MSG_ERROR "Sitio invalido\n" RESET);
                break;
            }

            mostrarSectoresDeSitio(&listaSitios[indiceSitio - 1]);
            break;

        case 2:
            mostrarSitios();
            printf(MENU_INPUT "Seleccione sitio: " RESET);
            if (!leerEnteroAdmin(NULL, &indiceSitio))
            {
                printf(MSG_ERROR "Entrada invalida\n" RESET);
                break;
            }

            if (indiceSitio < 1 || indiceSitio > cantidadSitios)
            {
                printf(MSG_ERROR "Sitio invalido\n" RESET);
                break;
            }

            printf(MENU_INPUT "Nombre del sector: " RESET);
            scanf(" %[^\n]", nombre);

            printf(MENU_INPUT "Inicial (letra): " RESET);
            scanf(" %c", &inicial);

            printf(MENU_INPUT "Cantidad de asientos: " RESET);
            if (!leerEnteroAdmin(NULL, &cantidad))
            {
                printf(MSG_ERROR "Entrada invalida\n" RESET);
                break;
            }

            agregarSectorASitio(
                &listaSitios[indiceSitio - 1],
                nombre,
                inicial,
                cantidad);
            break;

        case 3:
            mostrarSitios();
            printf(MENU_INPUT "Seleccione sitio: " RESET);
            if (!leerEnteroAdmin(NULL, &indiceSitio))
            {
                printf(MSG_ERROR "Entrada invalida\n" RESET);
                break;
            }

            if (indiceSitio < 1 || indiceSitio > cantidadSitios)
            {
                printf(MSG_ERROR "Sitio invalido\n" RESET);
                break;
            }

            resetSectoresDeSitio(&listaSitios[indiceSitio - 1]);
            break;

        case 4:
            printf(MSG_INFO "Volviendo...\n" RESET);
            break;

        default:
            printf(MSG_ERROR "Opcion invalida\n" RESET);
        }

    } while (opcion != 4);
}

// ==================== ESTADÍSTICAS ====================

/*
 * Objetivo: Mostrar top 3 de mes-anio con mayor cantidad de eventos.
 * Entradas: No recibe parametros; usa arreglo global de eventos.
 * Salidas: impresion de ranking ordenado de mayor a menor.
 * Restricciones: requiere eventos cargados para producir resultados.
 */
static void mostrarTopMeses()
{
    struct MesCount
    {
        char mesAnio[10];
        int count;
    };

    // Contar eventos por mes-año
    struct MesCount *meses = NULL;
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
            struct MesCount *tmp = realloc(meses, (mesesCount + 1) * sizeof(struct MesCount));
            if (tmp == NULL)
            {
                free(meses);
                printf(MSG_ERROR "Error de memoria en estadisticas.\n" RESET);
                return;
            }
            meses = tmp;
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

    free(meses);
}

/*
 * Objetivo: Mostrar top 3 de productoras con mayor recaudacion total.
 * Entradas: No recibe parametros; lee facturas del archivo configurado.
 * Salidas: impresion de ranking de productoras por monto.
 * Restricciones: depende de facturas validas en almacenamiento.
 */
static void mostrarTopProductoras()
{
    struct ProdTotal
    {
        char nombre[MAX_PRODUCTORA];
        float total;
    };

    Factura *facturas = NULL;
    int cantidadFacturas = 0;

    if (!facturaCargarTodas(ARCHIVO_FACTURAS, &facturas, &cantidadFacturas) || cantidadFacturas == 0)
    {
        printf(MSG_WARNING "No hay facturas registradas.\n" RESET);
        return;
    }

    struct ProdTotal *prod = NULL;
    int prodCount = 0;

    for (int i = 0; i < cantidadFacturas; i++)
    {
        int idx = -1;
        for (int j = 0; j < prodCount; j++)
        {
            if (strcmp(prod[j].nombre, facturas[i].productora) == 0)
            {
                idx = j;
                break;
            }
        }

        if (idx == -1)
        {
            struct ProdTotal *tmp = realloc(prod, (prodCount + 1) * sizeof(struct ProdTotal));
            if (tmp == NULL)
            {
                free(prod);
                facturaLiberarLista(facturas);
                printf(MSG_ERROR "Error de memoria en estadisticas.\n" RESET);
                return;
            }
            prod = tmp;
            idx = prodCount;
            strcpy(prod[idx].nombre, facturas[i].productora);
            prod[idx].total = 0.0;
            prodCount++;
        }

        prod[idx].total += facturas[i].total;
    }

    facturaLiberarLista(facturas);

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

    free(prod);
}

/*
 * Objetivo: Listar sitios con cantidad de eventos y total recaudado.
 * Entradas: No recibe parametros; combina datos de eventos y facturas.
 * Salidas: tabla ordenada por recaudacion descendente.
 * Restricciones: requiere consistencia entre nombres de sitio en eventos/facturas.
 */
static void mostrarSitiosConEventosYRecaudacion()
{
    struct SitioData
    {
        char nombre[MAX_NOMBRE];
        int eventos;
        float recaudacion;
    };

    // 1. Contar eventos por sitio
    struct SitioData *sitios = NULL;
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
            struct SitioData *tmp = realloc(sitios, (sitiosCount + 1) * sizeof(struct SitioData));
            if (tmp == NULL)
            {
                free(sitios);
                printf(MSG_ERROR "Error de memoria en estadisticas.\n" RESET);
                return;
            }
            sitios = tmp;
            idx = sitiosCount;
            strcpy(sitios[idx].nombre, sitioNombre);
            sitios[idx].eventos = 0;
            sitios[idx].recaudacion = 0.0;
            sitiosCount++;
        }
        sitios[idx].eventos++;
    }

    // 2. Leer facturas para sumar recaudación por sitio
    Factura *facturas = NULL;
    int cantidadFacturas = 0;
    if (facturaCargarTodas(ARCHIVO_FACTURAS, &facturas, &cantidadFacturas))
    {
        for (int f = 0; f < cantidadFacturas; f++)
        {
            for (int i = 0; i < sitiosCount; i++)
            {
                if (strcmp(sitios[i].nombre, facturas[f].sitio) == 0)
                {
                    sitios[i].recaudacion += facturas[f].total;
                    break;
                }
            }
        }
        facturaLiberarLista(facturas);
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

    free(sitios);
}

/*
 * Objetivo: Ejecutar el bloque completo de estadisticas administrativas.
 * Entradas: No recibe parametros.
 * Salidas: muestra las tres estadisticas requeridas por especificacion.
 * Restricciones: depende de datos cargados en memoria y facturas en archivo.
 */
void mostrarEstadisticas()
{
    mostrarTopMeses();
    mostrarTopProductoras();
    mostrarSitiosConEventosYRecaudacion();
}

/*
 * Objetivo: Mostrar y gestionar el menu principal de administracion.
 * Entradas: opcion de menu ingresada por consola.
 * Salidas: direcciona hacia submenus o vuelve al menu principal.
 * Restricciones: acceso esperado despues de login exitoso.
 */
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

        if (!leerEnteroAdmin(MENU_INPUT "Seleccione: " RESET, &opcion))
        {
            printf(MSG_ERROR "Entrada invalida\n" RESET);
            continue;
        }

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
            facturaMostrarListadoAdmin(ARCHIVO_FACTURAS);
            break;

        case 6:
            mostrarEstadisticas();
            break;
        case 7:
            printf(MSG_INFO "Volviendo...\n" RESET);
            break;

        default:
            printf(MSG_ERROR "Opcion invalida\n" RESET);
        }

    } while (opcion != 7);
}

/*
 * Objetivo: Liberar la memoria del listado de usuarios administradores.
 * Entradas: No recibe parametros.
 * Salidas: libera listaUsuarios y reinicia contador.
 * Restricciones: segura ante lista vacia (free(NULL) es valido).
 */
void liberarUsuarios()
{
    free(listaUsuarios);
    listaUsuarios = NULL;
    cantidadUsuarios = 0;
}