#ifndef PDF_FACTURA_H
#define PDF_FACTURA_H

#include <stddef.h>
#include "factura.h"

/* Directorio por defecto para almacenar facturas PDF. */
#define FACTURA_PDF_DIR "data/pdfs"

/*
 * Objetivo: Generar un archivo PDF con el detalle de una factura.
 * Entradas:
 *   - factura: puntero a la factura origen.
 *   - directorioSalida: ruta de carpeta destino (usa FACTURA_PDF_DIR si es NULL/vacio).
 *   - rutaGenerada: buffer opcional para retornar la ruta final del archivo generado.
 *   - tamRuta: tamano del buffer rutaGenerada.
 * Salida:
 *   - Retorna 1 si genera el PDF correctamente, 0 si falla.
 * Restricciones:
 *   - factura no debe ser NULL.
 */
int facturaGenerarPDF(const Factura *factura, const char *directorioSalida, char *rutaGenerada, size_t tamRuta);

#endif