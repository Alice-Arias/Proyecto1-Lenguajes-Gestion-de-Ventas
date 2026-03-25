#ifndef EVENTO_H
#define EVENTO_H

#include "sitio.h"   
#include <time.h>    

#define MAX_NOMBRE_EVENTO 100
#define MAX_PRODUCTORA 100
#define MAX_FECHA 11   // DD/MM/YYYY + null

typedef struct {
    char nombreSector[MAX_SECTOR_NOMBRE]; 
    float precio;
} PrecioSector;

// Estructura para un evento
typedef struct {
    char nombre[MAX_NOMBRE_EVENTO];
    char productora[MAX_PRODUCTORA];
    char fecha[MAX_FECHA];                
    SitioEvento *sitio;                  
    PrecioSector *precios;                
    int **disponibilidad;                 
} Evento;

extern Evento *eventos;
extern int totalEventos;

//Ver si se cambia el manejo de precios 

// Funciones para manejo de eventos
void inicializarEventos();
void crearEvento(const char *nombre, const char *productora, const char *fecha, SitioEvento *sitio, float *preciosPorSector);
void liberarEvento(Evento *evento);
void liberarEventos();
void guardarEventosEnArchivo(const char *ruta);
void cargarEventosDesdeArchivo(const char *ruta);
void mostrarEventos();

int verificarDisponibilidad(Evento *evento, int sectorIdx, int asientoIdx);
void marcarVendido(Evento *evento, int sectorIdx, int asientoIdx);
float obtenerPrecioAsiento(Evento *evento, int sectorIdx, int asientoIdx); 

#endif