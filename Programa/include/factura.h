#ifndef FACTURA_H
#define FACTURA_H

#include <stddef.h>
#include "evento.h"

#define ARCHIVO_FACTURAS "data/facturas.txt"
#define MAX_CEDULA_FACTURA 50
#define MAX_CLIENTE_FACTURA 100
#define MAX_DETALLE_FACTURA 2048

typedef struct
{
    int id;
    char cedula[MAX_CEDULA_FACTURA];
    char cliente[MAX_CLIENTE_FACTURA];
    char evento[MAX_NOMBRE_EVENTO];
    char productora[MAX_PRODUCTORA];
    char sitio[MAX_NOMBRE];
    char fechaCompra[MAX_FECHA];
    char fechaEvento[MAX_FECHA];
    char detalleAsientos[MAX_DETALLE_FACTURA];
    float subtotal;
    float servicio;
    float total;
} Factura;

int facturaCrearDesdeCompra(Factura *factura, const Evento *evento, const char *cedula, const char *cliente,
                            float subtotal, float servicio, float total, const char *detalleAsientos, int idSugerido);
int facturaGuardar(const char *ruta, Factura *factura);
int facturaCargarTodas(const char *ruta, Factura **facturas, int *cantidad);
void facturaLiberarLista(Factura *facturas);
void facturaMostrarDetalle(const Factura *factura);
void facturaMostrarListadoAdmin(const char *ruta);
float facturaTotalPorEvento(const char *ruta, const char *nombreEvento);

#endif