#ifndef ASIENTO_H
#define ASIENTO_H

// Un asiento solo tiene un código identificador (ej. A1, B3, etc)
typedef struct {
    char *codigo;  
} Asiento;

// Funciones para manejo de arreglos de asientos
Asiento* crearAsientos(int cantidad, char inicial);

// Libera la memoria de un arreglo de asientos
void liberarAsientos(Asiento *asientos, int cantidad);

// Muestra todos los asientos 
void mostrarAsientos(const Asiento *asientos, int cantidad);

#endif