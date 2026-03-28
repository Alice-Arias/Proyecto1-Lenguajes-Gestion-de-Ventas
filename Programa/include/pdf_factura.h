#ifndef PDF_FACTURA_H
#define PDF_FACTURA_H

#include <stddef.h>
#include "factura.h"

#define FACTURA_PDF_DIR "data/pdfs"

int facturaGenerarPDF(const Factura *factura, const char *directorioSalida, char *rutaGenerada, size_t tamRuta);

#endif