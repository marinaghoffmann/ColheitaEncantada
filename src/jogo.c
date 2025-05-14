#include "../include/jogo.h"
#include "../include/planta.h"
#include "../include/objetivo.h"
#include "../include/item_magico.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void avancarDia(Fila* fila) {
    Planta* atual = fila->inicio;
    while (atual != NULL) {
        if (atual->dias_para_colher > 0) {
            atual->dias_para_colher--;
        }
        atual = atual->prox;
    }
    printf("🌞 Um dia se passou. As plantas cresceram!\n");
}

void colher(Fila* fila) {
    int colheuAlguma = 0;

    while (fila->inicio != NULL && fila->inicio->dias_para_colher == 0) {
        Planta* colhida = fila->inicio;
        printf("🧺 Colhendo planta: %s\n", colhida->nome); 
        fila->inicio = fila->inicio->prox;
        free(colhida);
        colheuAlguma = 1;
    }

    if (fila->inicio == NULL) {
        fila->fim = NULL;
    }

    if (!colheuAlguma) {
        if (fila->inicio != NULL) {
            printf("⏳ A próxima planta (%s) ainda não está pronta. Faltam %d dias.\n",
                   fila->inicio->nome,
                   fila->inicio->dias_para_colher);
        } else {
            printf("🌾 Nenhuma planta disponível para colheita no momento.\n");
        }
    }
}

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

void usarItemMagico(Fila *fila) {
    srand(time(NULL));
    int indice_item = rand() % NUM_ITENS_MAGICOS;
    const ItemMagico *item = &itens_magicos[indice_item];

    printf("✨ Item mágico disponível: %s\n", item->nome);
    printf("📜 Descrição: %s\n", item->descricao);
    printf("Deseja usar este item? (s/n): ");
    char escolha;
    scanf(" %c", &escolha);

    if (escolha == 's' || escolha == 'S') {
        Planta *atual = fila->inicio;
        int afetouAlguma = 0;

        while (atual != NULL) {
            int impacto = item->efeito * item->impacto;
            atual->dias_para_colher += impacto;

            if (atual->dias_para_colher < 0) {
                atual->dias_para_colher = 0;
            }

            if (impacto > 0) {
                printf("🌟 O item %s acelerou a planta '%s' em %d dia(s)!\n", item->nome, atual->nome, impacto);
            } else if (impacto < 0) {
                printf("⚠️ O item %s prejudicou a planta '%s', atrasando-a em %d dia(s)!\n", item->nome, atual->nome, -impacto);
            }

            atual = atual->prox;
            afetouAlguma = 1;
        }

        if (!afetouAlguma) {
            printf("🌱 Nenhuma planta foi afetada pelo item mágico.\n");
        }
    } else {
        printf("🔮 Você decidiu não usar o item mágico.\n");
    }
}