#ifndef ASIENTO_H
#define ASIENTO_H

// Un asiento solo tiene un código identificador (ej. A1, B3, etc)
typedef struct
{
    char *codigo;
} Asiento;

// Funciones para manejo de arreglos de asientos
Asiento *crearAsientos(int cantidad, char inicial);
void liberarAsientos(Asiento *asientos, int cantidad);
void mostrarAsientos(const Asiento *asientos, int cantidad);
void guardarAsientosEnArchivo(const char *ruta);
void cargarAsientosDesdeArchivo(const char *ruta);
void sincronizarAsientosConFacturas(const char *rutaFacturas);
#endif