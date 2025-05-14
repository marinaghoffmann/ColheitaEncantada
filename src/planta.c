#include "../include/planta.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void inicializarFila(Fila* fila) {
    fila->inicio = NULL;
    fila->fim = NULL;
}

Planta* criarPlanta(const char* nome, const char* efeito, const char* necessidades, int dias) {
    Planta* nova = (Planta*)malloc(sizeof(Planta));
    if (nova == NULL) {
        printf("Erro ao alocar memória para a planta.\n");
        return NULL;
    }
    strcpy(nova->nome, nome);
    strcpy(nova->efeito, efeito);
    strcpy(nova->necessidades, necessidades); 
    nova->dias_para_colher = dias;
    nova->prox = NULL;
    return nova;
}

void listarPlantas(Fila fila) {
    if (fila.inicio == NULL) {
        printf("Nenhuma planta na fila.\n");
        return;
    }

    Planta* atual = fila.inicio;
    printf("📋 Plantas na fila:\n");
    while (atual != NULL) {
        printf("- %s | Dias para colher: %d | Necessidades: %s | Efeito: %s\n",
               atual->nome, atual->dias_para_colher, atual->necessidades, atual->efeito);
        atual = atual->prox;
    }
}

void liberarFila(Fila* fila) {
    Planta* atual = fila->inicio;
    while (atual != NULL) {
        Planta* temp = atual;
        atual = atual->prox;
        free(temp);
    }
    fila->inicio = fila->fim = NULL;
}