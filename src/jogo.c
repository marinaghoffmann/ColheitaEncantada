#include "../include/jogo.h"
#include "../include/planta.h"
#include "../include/objetivo.h"
#include "../include/item_magico.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


int verificarObjetivo(Fila *colhidas, const Objetivo *objetivo) {
    int encontrou_primeira = 0;
    int encontrou_segunda = 0;

    Planta *atual = colhidas->inicio;
    while (atual != NULL) {
        if (strcmp(atual->nome, objetivo->nome1) == 0) {
            encontrou_primeira = 1;
        }
        if (strcmp(atual->nome, objetivo->nome2) == 0) {
            encontrou_segunda = 1;
        }
        atual = atual->prox;
    }

    return encontrou_primeira && encontrou_segunda;
}

