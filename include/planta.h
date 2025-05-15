#ifndef PLANTA_H
#define PLANTA_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Planta {
    char nome[100];
    char efeito[300];
    char necessidades[300];
    int dias_para_colher;
    struct Planta* prox;
} Planta;

typedef struct {
    Planta* inicio;
    Planta* fim;
} Fila;

// nossas funções de manipulação da fila e plantas
void inicializarFila(Fila* fila);
void liberarFila(Fila* fila);
void enqueue(Fila* fila, Planta* nova); 
Planta* dequeue(Fila* fila);      
void checarPlantasColhidas(Fila* colhidas); 

// nossas funções de lógica do jogo/planta
Planta* criarPlanta(const char* nome, const char* efeito, const char* necessidades, int dias);
void listarPlantas(Fila fila);
void ordenarPlantasPorColheita(Fila* fila);
int efeitoValido(const char *efeito);
const char* escolherEfeitoRotativo(void);

#endif