#include "../include/planta.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

const char *efeitos_permitidos[] = {"cura", "energia", "clarividência", "neutralizar venenos", "proteção"};
#define NUM_EFEITOS 5

static int indice_efeito_atual = 0;

int efeitoValido(const char *efeito) {
    for (int i = 0; i < NUM_EFEITOS; i++) {
        if (strcasecmp(efeito, efeitos_permitidos[i]) == 0) {
            return 1; // Efeito válido
        }
    }
    return 0;
}

const char* escolherEfeitoRotativo() {
    if (indice_efeito_atual < 0 || indice_efeito_atual >= NUM_EFEITOS) {
        indice_efeito_atual = 0; 
    }
    const char* efeito = efeitos_permitidos[indice_efeito_atual];
    indice_efeito_atual = (indice_efeito_atual + 1) % NUM_EFEITOS; 
    return efeito;
}

void inicializarFila(Fila* fila) {
    fila->inicio = NULL;
    fila->fim = NULL;
}

Planta* criarPlanta(const char* nome, const char* efeito, const char* necessidades, int dias) {
    if (nome == NULL || efeito == NULL || necessidades == NULL || dias < 0) {
        printf("❌ Erro: Parâmetros inválidos ao criar a planta.\n");
        return NULL;
    }

    Planta* nova = (Planta*)malloc(sizeof(Planta));
    if (nova == NULL) {
        printf("❌ Erro ao alocar memória para a planta.\n");
        return NULL;
    }

    strncpy(nova->nome, nome, sizeof(nova->nome) - 1);
    nova->nome[sizeof(nova->nome) - 1] = '\0'; 
    strncpy(nova->efeito, efeito, sizeof(nova->efeito) - 1);
    nova->efeito[sizeof(nova->efeito) - 1] = '\0';
    strncpy(nova->necessidades, necessidades, sizeof(nova->necessidades) - 1);
    nova->necessidades[sizeof(nova->necessidades) - 1] = '\0'; 
    nova->dias_para_colher = dias;
    nova->prox = NULL;

    return nova;
}

/*void listarPlantas(Fila fila) {
 ///   printf("📋 Plantas na fila (ordem crescente de tempo de colheita):\n\n");
//
  //  ordenarPlantasPorColheita(&fila);
//
  //  Planta* atual = fila.inicio;
    //if (atual == NULL) {
      //  printf("  Nenhuma planta na fila.\n");
    //} else {
      //  while (atual != NULL) {
            printf("  🌿 Nome: %s\n", atual->nome);
            printf("  💌 Efeito: %s\n", atual->efeito);
            printf("  ✨ Necessidades: %s\n", atual->necessidades);
            printf("  📅 Dias para Colher: %d\n\n", atual->dias_para_colher);
            atual = atual->prox;
        }
    }
}
*/
void liberarFila(Fila* fila) {
    Planta* atual = fila->inicio;
    while (atual != NULL) {
        Planta* temp = atual;
        atual = atual->prox;
        free(temp);
    }
    fila->inicio = fila->fim = NULL;
}

void ordenarPlantasPorColheita(Fila* fila) {
    if (fila->inicio == NULL || fila->inicio->prox == NULL) {
        return;
    }

    Planta* sorted = NULL; 
    Planta* atual = fila->inicio;

    while (atual != NULL) {
        Planta* prox = atual->prox; 
        if (sorted == NULL || atual->dias_para_colher < sorted->dias_para_colher) {
            
            atual->prox = sorted;
            sorted = atual;
        } else {
            
            Planta* temp = sorted;
            while (temp->prox != NULL && temp->prox->dias_para_colher <= atual->dias_para_colher) {
                temp = temp->prox;
            }
            atual->prox = temp->prox;
            temp->prox = atual;
        }
        atual = prox; 
    }

    fila->inicio = sorted; 
}