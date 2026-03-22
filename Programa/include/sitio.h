#ifndef SITIO_H
#define SITIO_H

#define MAX_NOMBRE 100
#define MAX_UBICACION 150
#define MAX_WEB 100

// Estructura para un sitio de eventos
typedef struct {
    char nombre[MAX_NOMBRE];
    char ubicacion[MAX_UBICACION];
    char web[MAX_WEB];
    // Agregar arreglo dinámico para Sector(por implementar)...
} SitioEvento;

// se declaran extern para que sean accesibles desde otros .c
extern SitioEvento *sitios;   
extern int totalSitios;       


void inicializarSitios();
void agregarSitio(const char *nombre, const char *ubicacion, const char *web);
void cargarSitiosDesdeArchivo(const char *ruta);
void guardarSitiosEnArchivo(const char *ruta);
void liberarSitios();
void mostrarSitios();

#endif