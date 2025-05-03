#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Planta {
    char nome[50];
    char efeito[100];
    int dias_para_colher;
    int valor_magico;
    struct Planta* prox;
} Planta;

typedef struct {
    Planta* inicio;
    Planta* fim;
} Fila;


void inicializarFila(Fila* fila) {
    fila->inicio = NULL;
    fila->fim = NULL;
}

Planta* criarPlanta(char nome[], char efeito[], int dias, int valor) {
    Planta* nova = (Planta*) malloc(sizeof(Planta));
    strcpy(nova->nome, nome);
    strcpy(nova->efeito, efeito);
    nova->dias_para_colher = dias;
    nova->valor_magico = valor;
    nova->prox = NULL;
    return nova;
}

void plantar(Fila* fila) { //usando FILA 
    char nome[50] = "Flor do Gelo"; // futuramente vindo da IA
    char efeito[100] = "Congela inimigos";
    int dias = rand() % 5 + 1;
    int valor = rand() % 100 + 1;

    Planta* nova = criarPlanta(nome, efeito, dias, valor);

    if (fila->fim == NULL) {
        fila->inicio = fila->fim = nova;
    } else {
        fila->fim->prox = nova;
        fila->fim = nova;
    }

    printf("🌱 Planta '%s' adicionada à fila!\n", nome);
}

void listarPlantas(Fila fila) {
    if (fila.inicio == NULL) {
        printf("Nenhuma planta na fila.\n");
        return;
    }

    Planta* atual = fila.inicio;
    printf("📋 Plantas na fila:\n");
    while (atual != NULL) {
        printf("- %s | Dias para colher: %d | Valor: %d | Efeito: %s\n",
               atual->nome, atual->dias_para_colher, atual->valor_magico, atual->efeito);
        atual = atual->prox;
    }
}

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

// Função para ordenar a fila de plantas por valor  mágico usando Insertion Sort
void insertionSort(Fila* fila) {
    Planta* sorted = NULL;
    Planta* atual = fila->inicio;

    while (atual != NULL) {
        Planta* proxima = atual->prox;

        if (sorted == NULL || atual->valor_magico < sorted->valor_magico) {
            atual->prox = sorted;
            sorted = atual;
        } else {
            Planta* temp = sorted;
            while (temp->prox != NULL && temp->prox->valor_magico <= atual->valor_magico) {
                temp = temp->prox;
            }
            atual->prox = temp->prox;
            temp->prox = atual;
        }

        atual = proxima;
    }

    fila->inicio = sorted;

    // Atualiza ponteiro para o fim
    fila->fim = fila->inicio;
    while (fila->fim != NULL && fila->fim->prox != NULL) {
        fila->fim = fila->fim->prox;
    }
}


void colher(Fila* fila) {
    int colheuAlguma = 0;

    // Ordenar as plantas prontas por valor
    insertionSort(fila);

    while (fila->inicio != NULL && fila->inicio->dias_para_colher == 0) {
        Planta* colhida = fila->inicio;
        printf("🧺 Colhendo planta: %s (valor: %d)\n", colhida->nome, colhida->valor_magico);
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

void liberarFila(Fila* fila) {
    Planta* atual = fila->inicio;
    while (atual != NULL) {
        Planta* temp = atual;
        atual = atual->prox;
        free(temp);
    }
    fila->inicio = fila->fim = NULL;
}


void menu() {
    printf("\n=== Agricultura Mágica ===\n");
    printf("1. Plantar nova semente\n");
    printf("2. Listar plantas\n");
    printf("3. Avançar um dia\n");
    printf("4. Colher plantas prontas\n");
    printf("0. Sair\n");
    printf("Escolha: ");
}

int main() {
    Fila fila;
    inicializarFila(&fila);
    int opcao;

    do {
        menu();
        scanf("%d", &opcao);
        getchar(); // limpar buffer

        switch (opcao) {
            case 1: plantar(&fila); break;
            case 2: listarPlantas(fila); break;
            case 3: avancarDia(&fila); break;
            case 4: colher(&fila); break;
            case 0: printf("Saindo do jogo...\n"); break;
            default: printf("Opção inválida.\n");
        }

    } while (opcao != 0);

    liberarFila(&fila);

    return 0;
}
