#include "../include/pdf_factura.h"

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <direct.h>
#define MKDIR(path) _mkdir(path)
#else
#include <sys/stat.h>
#include <sys/types.h>
#define MKDIR(path) mkdir(path, 0777)
#endif

#define PDF_STREAM_MAX 16384
#define PDF_LINE_MAX 256
#define PDF_WRAP_WIDTH 78

typedef struct {
    char texto[PDF_WRAP_WIDTH + 1];
} LineaEnvuelta;

static int escaparPDF(const char *entrada, char *salida, size_t tamSalida);
static int appendf(char *destino, size_t tamDestino, size_t *usado, const char *fmt, ...);

/*
 * Objetivo: Agregar una linea de texto al stream PDF con salto vertical.
 * Entradas:
 *   - stream/tamStream/usado: buffer de salida y control de posicion actual.
 *   - linea: texto a escribir en el PDF.
 * Salida:
 *   - 1 si la linea se agrego correctamente, 0 en caso de error.
 * Restricciones:
 *   - La linea se escapa para PDF antes de escribirla.
 *   - Falla si no hay espacio suficiente en el stream.
 */
static int agregarLineaTexto(char *stream, size_t tamStream, size_t *usado, const char *linea)
{
    char escapada[PDF_LINE_MAX * 2];

    if (!escaparPDF(linea, escapada, sizeof(escapada))) {
        return 0;
    }

    return appendf(stream, tamStream, usado, "(%s) Tj\n0 -14 Td\n", escapada);
}

/*
 * Objetivo: Escapar caracteres especiales para texto literal de PDF.
 * Entradas:
 *   - entrada: texto original.
 *   - salida/tamSalida: buffer destino para texto escapado.
 * Salida:
 *   - 1 si el escape fue exitoso, 0 si hay parametros invalidos o desborde.
 * Restricciones:
 *   - Escapa '(', ')' y '\\'.
 *   - Caracteres de control (<32) se reemplazan por espacio.
 */
static int escaparPDF(const char *entrada, char *salida, size_t tamSalida)
{
    size_t i = 0;
    size_t j = 0;

    if (entrada == NULL || salida == NULL || tamSalida == 0) {
        return 0;
    }

    while (entrada[i] != '\0') {
        char c = entrada[i++];
        if (c == '(' || c == ')' || c == '\\') {
            if (j + 2 >= tamSalida) {
                return 0;
            }
            salida[j++] = '\\';
            salida[j++] = c;
        } else if ((unsigned char)c < 32) {
            if (j + 1 >= tamSalida) {
                return 0;
            }
            salida[j++] = ' ';
        } else {
            if (j + 1 >= tamSalida) {
                return 0;
            }
            salida[j++] = c;
        }
    }

    salida[j] = '\0';
    return 1;
}

/*
 * Objetivo: Concatenar texto formateado al buffer destino de forma segura.
 * Entradas:
 *   - destino/tamDestino/usado: buffer de salida y cantidad ya usada.
 *   - fmt y argumentos variables: formato estilo printf.
 * Salida:
 *   - 1 si se pudo escribir, 0 en error o si no hay espacio.
 * Restricciones:
 *   - Usa vsnprintf para evitar desbordes.
 */
static int appendf(char *destino, size_t tamDestino, size_t *usado, const char *fmt, ...)
{
    va_list args;
    int escritos;

    if (destino == NULL || usado == NULL || fmt == NULL) {
        return 0;
    }

    if (*usado >= tamDestino) {
        return 0;
    }

    va_start(args, fmt);
    escritos = vsnprintf(destino + *usado, tamDestino - *usado, fmt, args);
    va_end(args);

    if (escritos < 0 || (size_t)escritos >= tamDestino - *usado) {
        return 0;
    }

    *usado += (size_t)escritos;
    return 1;
}

/*
 * Objetivo: Crear el directorio de salida para PDFs si aun no existe.
 * Entradas:
 *   - ruta: ruta del directorio a crear.
 * Salida:
 *   - 1 si el directorio existe o se pudo crear, 0 en caso contrario.
 * Restricciones:
 *   - Si la ruta es nula o vacia retorna error.
 */
static int crearDirectorioSiNoExiste(const char *ruta)
{
    int resultado;

    if (ruta == NULL || ruta[0] == '\0') {
        return 0;
    }

    resultado = MKDIR(ruta);
    if (resultado == 0 || errno == EEXIST) {
        return 1;
    }

    return 0;
}

/*
 * Objetivo: Dividir texto largo en varias lineas de ancho fijo para el PDF.
 * Entradas:
 *   - texto: cadena a envolver.
 *   - lineas: arreglo donde se almacenan las lineas resultantes.
 *   - maxLineas: capacidad maxima del arreglo lineas.
 * Salida:
 *   - Cantidad de lineas generadas.
 * Restricciones:
 *   - Se intenta cortar por espacio para no partir palabras.
 */
static int envolverTexto(const char *texto, LineaEnvuelta *lineas, int maxLineas)
{
    int cantidad = 0;
    const char *cursor = texto;

    if (texto == NULL || lineas == NULL || maxLineas <= 0) {
        return 0;
    }

    while (*cursor != '\0' && cantidad < maxLineas) {
        while (*cursor == ' ') {
            cursor++;
        }

        if (*cursor == '\0') {
            break;
        }

        int largo = 0;
        int ultimoEspacio = -1;

        while (cursor[largo] != '\0' && largo < PDF_WRAP_WIDTH) {
            if (cursor[largo] == ' ') {
                ultimoEspacio = largo;
            }
            largo++;
        }

        if (cursor[largo] != '\0' && ultimoEspacio > 0) {
            largo = ultimoEspacio;
        }

        memcpy(lineas[cantidad].texto, cursor, (size_t)largo);
        lineas[cantidad].texto[largo] = '\0';
        cantidad++;

        cursor += largo;
    }

    return cantidad;
}

/*
 * Objetivo: Escribir un objeto indirecto PDF en el archivo.
 * Entradas:
 *   - archivo: puntero al archivo PDF abierto.
 *   - numeroObjeto: identificador numerico del objeto.
 *   - contenido: contenido del objeto (diccionario o stream ya preparado).
 * Salida:
 *   - 1 si la escritura fue exitosa, 0 en caso de error.
 * Restricciones:
 *   - Archivo y contenido deben ser validos.
 */
static int escribirObjetoPDF(FILE *archivo, int numeroObjeto, const char *contenido)
{
    if (archivo == NULL || contenido == NULL) {
        return 0;
    }

    if (fprintf(archivo, "%d 0 obj\n%s\nendobj\n", numeroObjeto, contenido) < 0) {
        return 0;
    }

    return 1;
}

/*
 * Objetivo: Generar una factura en formato PDF simple (PDF 1.4) desde una Factura.
 * Entradas:
 *   - factura: datos de factura a exportar.
 *   - directorioSalida: carpeta destino; si es NULL/vacio usa FACTURA_PDF_DIR.
 *   - rutaGenerada/tamRuta: buffer opcional para devolver la ruta final.
 * Salida:
 *   - 1 si el PDF se genero correctamente, 0 si ocurrio algun error.
 * Restricciones:
 *   - Requiere factura valida.
 *   - El stream de contenido esta limitado por PDF_STREAM_MAX.
 */
int facturaGenerarPDF(const Factura *factura, const char *directorioSalida, char *rutaGenerada, size_t tamRuta)
{
    char rutaLocal[260];
    char stream[PDF_STREAM_MAX];
    LineaEnvuelta lineasDetalle[40];
    long offsets[6];
    long inicioXref;
    FILE *archivo;
    size_t usado = 0;
    int cantidadLineas;
    int i;

    if (factura == NULL) {
        return 0;
    }

    if (directorioSalida == NULL || directorioSalida[0] == '\0') {
        directorioSalida = FACTURA_PDF_DIR;
    }

    if (!crearDirectorioSiNoExiste(directorioSalida)) {
        return 0;
    }

    if (snprintf(rutaLocal, sizeof(rutaLocal), "%s/factura_%06d.pdf", directorioSalida, factura->id) >= (int)sizeof(rutaLocal)) {
        return 0;
    }

    if (rutaGenerada != NULL && tamRuta > 0) {
        snprintf(rutaGenerada, tamRuta, "%s", rutaLocal);
    }

    if (!appendf(stream, sizeof(stream), &usado, "BT\n/F1 18 Tf\n50 800 Td\n(FACTURA) Tj\nET\n")) {
        return 0;
    }

    if (!appendf(stream, sizeof(stream), &usado, "BT\n/F1 11 Tf\n50 772 Td\n")) {
        return 0;
    }

    {
        char tmp[32];
        snprintf(tmp, sizeof(tmp), "%d", factura->id);
        if (!appendf(stream, sizeof(stream), &usado, "(ID: %s) Tj\n0 -14 Td\n", tmp)) {
            return 0;
        }
    }

    {
        char linea[PDF_LINE_MAX];

        snprintf(linea, sizeof(linea), "Cedula: %s", factura->cedula);
        if (!agregarLineaTexto(stream, sizeof(stream), &usado, linea)) return 0;

        snprintf(linea, sizeof(linea), "Nombre: %s", factura->cliente);
        if (!agregarLineaTexto(stream, sizeof(stream), &usado, linea)) return 0;

        snprintf(linea, sizeof(linea), "Evento: %s", factura->evento);
        if (!agregarLineaTexto(stream, sizeof(stream), &usado, linea)) return 0;

        snprintf(linea, sizeof(linea), "Productora: %s", factura->productora);
        if (!agregarLineaTexto(stream, sizeof(stream), &usado, linea)) return 0;

        snprintf(linea, sizeof(linea), "Sitio: %s", factura->sitio);
        if (!agregarLineaTexto(stream, sizeof(stream), &usado, linea)) return 0;

        snprintf(linea, sizeof(linea), "Fecha compra: %s", factura->fechaCompra);
        if (!agregarLineaTexto(stream, sizeof(stream), &usado, linea)) return 0;

        snprintf(linea, sizeof(linea), "Fecha evento: %s", factura->fechaEvento);
        if (!agregarLineaTexto(stream, sizeof(stream), &usado, linea)) return 0;
    }

    if (!appendf(stream, sizeof(stream), &usado, "(Detalle de asientos:) Tj\n0 -14 Td\n")) {
        return 0;
    }

    cantidadLineas = envolverTexto(factura->detalleAsientos, lineasDetalle, (int)(sizeof(lineasDetalle) / sizeof(lineasDetalle[0])));
    if (cantidadLineas == 0) {
        cantidadLineas = 1;
        snprintf(lineasDetalle[0].texto, sizeof(lineasDetalle[0].texto), "N/A");
    }

    for (i = 0; i < cantidadLineas; i++) {
        if (!agregarLineaTexto(stream, sizeof(stream), &usado, lineasDetalle[i].texto)) {
            return 0;
        }
    }

    {
        char linea[PDF_LINE_MAX];

        snprintf(linea, sizeof(linea), "Subtotal: %.2f", factura->subtotal);
        if (!agregarLineaTexto(stream, sizeof(stream), &usado, linea)) return 0;

        snprintf(linea, sizeof(linea), "Servicio (5%%): %.2f", factura->servicio);
        if (!agregarLineaTexto(stream, sizeof(stream), &usado, linea)) return 0;

        snprintf(linea, sizeof(linea), "TOTAL: %.2f", factura->total);
        if (!agregarLineaTexto(stream, sizeof(stream), &usado, linea)) return 0;

        if (!appendf(stream, sizeof(stream), &usado, "ET\n")) {
            return 0;
        }
    }

    archivo = fopen(rutaLocal, "wb");
    if (archivo == NULL) {
        return 0;
    }

    if (fprintf(archivo, "%%PDF-1.4\n") < 0) {
        fclose(archivo);
        return 0;
    }

    offsets[1] = ftell(archivo);
    if (!escribirObjetoPDF(archivo, 1, "<< /Type /Catalog /Pages 2 0 R >>")) {
        fclose(archivo);
        return 0;
    }

    offsets[2] = ftell(archivo);
    if (!escribirObjetoPDF(archivo, 2, "<< /Type /Pages /Count 1 /Kids [3 0 R] >>")) {
        fclose(archivo);
        return 0;
    }

    offsets[3] = ftell(archivo);
    if (!escribirObjetoPDF(archivo, 3, "<< /Type /Page /Parent 2 0 R /MediaBox [0 0 595 842] /Resources << /Font << /F1 4 0 R >> >> /Contents 5 0 R >>")) {
        fclose(archivo);
        return 0;
    }

    offsets[4] = ftell(archivo);
    if (!escribirObjetoPDF(archivo, 4, "<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>")) {
        fclose(archivo);
        return 0;
    }

    offsets[5] = ftell(archivo);
    if (fprintf(archivo, "5 0 obj\n<< /Length %zu >>\nstream\n%sendstream\nendobj\n", usado, stream) < 0) {
        fclose(archivo);
        return 0;
    }

    inicioXref = ftell(archivo);
    if (inicioXref < 0) {
        fclose(archivo);
        return 0;
    }

    if (fprintf(archivo, "xref\n0 6\n") < 0 ||
        fprintf(archivo, "0000000000 65535 f \n") < 0) {
        fclose(archivo);
        return 0;
    }

    for (i = 1; i <= 5; i++) {
        if (fprintf(archivo, "%010ld 00000 n \n", offsets[i]) < 0) {
            fclose(archivo);
            return 0;
        }
    }

    if (fprintf(archivo, "trailer\n<< /Size 6 /Root 1 0 R >>\nstartxref\n%ld\n%%%%EOF\n", inicioXref) < 0) {
        fclose(archivo);
        return 0;
    }

    fclose(archivo);
    return 1;
}
