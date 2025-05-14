#ifndef PLANTA_H
#define PLANTA_H

#include "api.h"

typedef struct Planta {
    char nome[100];
    char efeito[300];
    char necessidades[300]; // Adicione o campo "necessidades"
    int dias_para_colher;
    struct Planta* prox;
} Planta;

typedef struct {
    Planta* inicio;
    Planta* fim;
} Fila;

void inicializarFila(Fila* fila);
Planta* criarPlanta(const char* nome, const char* efeito, const char* necessidades, int dias);
void listarPlantas(Fila fila);
void liberarFila(Fila* fila);

#endif