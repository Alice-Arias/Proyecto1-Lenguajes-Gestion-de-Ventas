#ifndef CLIENTE_H
#define CLIENTE_H

#include "../include/evento.h"
#include "../include/sitio.h"

int seleccionarSector(Evento *evento);
int seleccionarAsientos(Evento *evento, int sectorIdx, int seleccionados[]);
float calcularSubtotal(Evento *evento, int sectorIdx, int asientos[], int cantidad);

void consultarEventos();
void mostrarDetallesEvento(Evento *evento);
void mostrarDetallesEventoTabla(Evento *evento);

void comprarBoletos();
void menuCliente();

#endif