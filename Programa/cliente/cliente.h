#ifndef CLIENTE_H
#define CLIENTE_H

#include "../include/evento.h"
#include "../include/sitio.h"

int seleccionarSector(Evento *evento);
int seleccionarAsientos(Evento *evento, int sectorIdx, int seleccionados[]);
float calcularSubtotal(Evento *evento, int sectorIdx, int asientos[], int cantidad);
void mostrarFactura(Evento *evento, char cedula[], char nombre[], float subtotal, float servicio, float total);
void guardarFactura(Evento *evento, char cedula[], char nombre[], float subtotal, float servicio, float total);

void comprarBoletos();
void menuCliente();

#endif