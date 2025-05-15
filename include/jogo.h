#ifndef JOGO_H
#define JOGO_H

#include "planta.h"
#include "objetivo.h"

void avancarDia(Fila *fila);
void colher(Fila *fila);
void usarItemMagico(Fila *fila);
int verificarObjetivo(Fila *colhidas, const Objetivo *objetivo);

#endif