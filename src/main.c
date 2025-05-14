#include "../include/planta.h"
#include "../include/jogo.h" // Inclua o cabeçalho de jogo
#include "../include/menu.h"
#include "../include/api.h"
#include "../include/objetivo.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include <curl/curl.h>

const char *prompt_text = 
    "Crie uma planta mágica com as seguintes características:\n"
    "- Um nome criativo e único.\n"
    "- Uma frase poética que descreve o efeito mágico da planta.\n"
    "- Uma frase mística que descreve as necessidades da planta, mencionando exatamente dois itens mágicos: sol, lua, adubo, chuva ou sombra.\n"
    "Formato de saída esperado:\n"
    "{ \"nome\": \"<nome da planta>\", \"efeito\": \"<frase do efeito>\", \"necessidades\": \"<frase das necessidades>\" }";

int main() {
    Fila fila;
    inicializarFila(&fila);
    int opcao;

    // Sorteio do objetivo
    srand(time(NULL)); // Inicializa o gerador de números aleatórios
    int indice_objetivo = rand() % NUM_OBJETIVOS;
    const Objetivo *objetivo_atual = &objetivos[indice_objetivo];

    printf("🎯 Objetivo da missão:\n");
    printf("%s\n\n", objetivo_atual->descricao);

    do {
        menu();
        scanf("%d", &opcao);
        getchar(); // limpar buffer

        switch (opcao) {
            case 1: {
                char resposta[512];
                if (consultar_api(prompt_text, resposta, sizeof(resposta)) == 0) {
                    char nome[100], efeito[300], necessidades[300];
                    sscanf(resposta, "%99[^|]|%299[^|]|%299[^\n]", nome, efeito, necessidades);

                    int dias_para_colher = 5 + rand() % 5; // Entre 5 e 9 dias

                    Planta* nova = criarPlanta(nome, efeito, necessidades, dias_para_colher);
                    if (nova != NULL) {
                        if (fila.fim == NULL) {
                            fila.inicio = fila.fim = nova;
                        } else {
                            fila.fim->prox = nova;
                            fila.fim = nova;
                        }
                        printf("🌱 Planta '%s' adicionada à fila com sucesso!\n", nome);
                        printf("✨ Efeito: %s\n", efeito);
                        printf("🌿 Necessidades: %s\n", necessidades);
                    }
                } else {
                    printf("❌ Erro ao consultar a API para gerar uma planta.\n");
                }
                break;
            }
            case 2:
                listarPlantas(fila);
                break;
            case 3:
                avancarDia(&fila);
                break;
            case 4:
                colher(&fila);
                break;
            case 0:
                if (verificarObjetivo(&fila, objetivo_atual)) {
                    printf("🎉 Parabéns! Você cumpriu o objetivo da missão!\n");
                } else {
                    printf("❌ Você não conseguiu cumprir o objetivo da missão.\n");
                }
                printf("Saindo do jogo...\n");
                break;
            default:
                printf("Opção inválida.\n");
        }

    } while (opcao != 0);

    liberarFila(&fila);

    return 0;
}