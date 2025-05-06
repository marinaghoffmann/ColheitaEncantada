#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/api.h" 
#include "/opt/homebrew/Cellar/cjson/1.7.18/include/cjson/cJSON.h"



void inicializarFila(Fila* fila) {
    fila->inicio = NULL;
    fila->fim = NULL;
}

Planta* criarPlanta(const char* nome, const char* efeito, int dias, int valor) {
    Planta* nova = (Planta*)malloc(sizeof(Planta));
    if (nova == NULL) {
        printf("Erro ao alocar memória para a planta.\n");
        return NULL;
    }
    strcpy(nova->nome, nome);
    strcpy(nova->efeito, efeito);
    nova->dias_para_colher = dias;
    nova->valor_magico = valor;
    nova->prox = NULL;
    return nova;
}

int consultar_nome_efeito_planta(char* nome, char* efeito) {
    const char* api_key = "AIzaSyC4MOaYgdeYs-NHQ4ysvZd4GbdfMizqDTI";
    const char* prompt_text = "Gere o nome e o efeito mágico de uma planta no seguinte formato:\n**Nome:** <nome>\n**Efeito Mágico:** <efeito>";

    char resposta[1024];  // ou outro tamanho adequado
    size_t tamanho_resposta = sizeof(resposta);

    int resultado = consultar_api(api_key, prompt_text, resposta, tamanho_resposta);
    if (resultado != 0) {
        fprintf(stderr, "Erro ao consultar a API.\n");
        return 1;
    }

    // Parse JSON com cJSON
    cJSON* root = cJSON_Parse(resposta);
    if (!root) {
        fprintf(stderr, "Erro ao parsear JSON.\n");
        return 1;
    }

    // Caminho: candidates[0].content.parts[0].text
    cJSON* candidates = cJSON_GetObjectItem(root, "candidates");
    if (!cJSON_IsArray(candidates)) goto erro;

    cJSON* cand0 = cJSON_GetArrayItem(candidates, 0);
    cJSON* content = cJSON_GetObjectItem(cand0, "content");
    cJSON* parts = cJSON_GetObjectItem(content, "parts");
    cJSON* part0 = cJSON_GetArrayItem(parts, 0);
    cJSON* text = cJSON_GetObjectItem(part0, "text");

    if (!cJSON_IsString(text)) goto erro;

    // Exemplo de resposta: "**Nome:** Rosa Solar\n**Efeito Mágico:** Cura ferimentos leves"
    const char* texto = text->valuestring;

    // Extrair nome e efeito
    char* pos_nome = strstr(texto, "**Nome:**");
    char* pos_efeito = strstr(texto, "**Efeito Mágico:**");

    if (!pos_nome || !pos_efeito) goto erro;

    pos_nome += strlen("**Nome:**");
    pos_efeito += strlen("**Efeito Mágico:**");

    // Remove possíveis quebras de linha ou espaços extras
    while (*pos_nome == ' ' || *pos_nome == '\n') pos_nome++;
    while (*pos_efeito == ' ' || *pos_efeito == '\n') pos_efeito++;

    // Copia o nome e efeito para os parâmetros
    int len_nome = pos_efeito - strlen("**Efeito Mágico:**") - pos_nome;
    strncpy(nome, pos_nome, len_nome);
    nome[len_nome] = '\0';
    strncpy(efeito, pos_efeito, 100);
    efeito[99] = '\0';

    cJSON_Delete(root);
    return 0;

erro:
    fprintf(stderr, "Erro ao extrair dados da resposta JSON.\n");
    cJSON_Delete(root);
    return 1;
}


// Função para plantar uma nova planta usando a API
void plantar(Fila* fila) {
    char nome[50];
    char efeito[100];
    int dias = rand() % 5 + 1;
    int valor = rand() % 100 + 1;

    // Consultar a API para obter o nome e o efeito mágico da planta
    if (consultar_nome_efeito_planta(nome, efeito) == 0) {
        Planta* nova = criarPlanta(nome, efeito, dias, valor);

        // Adicionar a nova planta à fila
        if (fila->fim == NULL) {
            fila->inicio = fila->fim = nova;
        } else {
            fila->fim->prox = nova;
            fila->fim = nova;
        }

        printf("🌱 Planta '%s' com efeito '%s' adicionada à fila!\n", nome, efeito);
    } else {
        printf("Erro ao consultar a API para obter o nome e o efeito da planta.\n");
    }
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

// Função para ordenar a fila de plantas por valor mágico usando Insertion Sort
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
