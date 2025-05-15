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

// Funções
void inicializarFila(Fila* fila);
void liberarFila(Fila* fila);
void enqueue(Fila* fila, Planta* nova); 
Planta* dequeue(Fila* fila);      
void checarPlantasColhidas(Fila* colhidas); 

#endif