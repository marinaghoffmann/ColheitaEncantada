#include "../include/planta.h"
#include "../include/jogo.h" 
#include "../include/menu.h"
#include "../include/api.h"
#include "../include/objetivo.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <cjson/cJSON.h>
#include <curl/curl.h>

int main() {
    Fila fila;
    inicializarFila(&fila);
    Fila colhidas;
    inicializarFila(&colhidas);
    int opcao;

    srand(time(NULL)); 
    int indice_objetivo = rand() % NUM_OBJETIVOS;
    const Objetivo *objetivo_atual = &objetivos[indice_objetivo];

    printf("\n\n📜 Enigma da Missão: '%s'\n\n", objetivo_atual->descricao);
    printf("🎯 Objetivo da Missão: Colher plantas com o efeito de %s e de %s! \n", objetivo_atual->nome1, objetivo_atual->nome2);

    int dias_restantes = 10; 

    do {
        printf("\n📅 Dias Restantes Na Missão: %d dias\n", dias_restantes);

        menu();
        scanf("%d", &opcao);
        getchar(); 
        
        switch (opcao) {
            case 1: {
                char resposta[512];
                char prompt_text[512];
                const char* efeito_escolhido = escolherEfeitoRotativo();
                snprintf(prompt_text, sizeof(prompt_text),
                         "Crie uma planta mágica com as seguintes características:\n"
                         "- Um nome criativo e místico ligado ao efeito mágico '%s'.\n"
                         "- Uma frase poética que descreve o efeito mágico da planta.\n"
                         "- Uma frase mística que descreve as necessidades da planta, mencionando exatamente dois itens mágicos: sol, lua, adubo, chuva ou sombra.\n"
                         "Formato de saída esperado:\n"
                         "{ \"nome\": \"<nome da planta>\", \"efeito\": \"<frase do efeito>\", \"necessidades\": \"<frase das necessidades>\" }",
                         efeito_escolhido);

                if (consultar_api(prompt_text, resposta, sizeof(resposta)) == 0) {
                    char nome[100], efeito[300], necessidades[300];
                    if (sscanf(resposta, "%99[^|]|%299[^|]|%299[^\n]", nome, efeito, necessidades) == 3) {
                        int dias_para_colher = 9 + rand() % 4; 
                        Planta* nova = criarPlanta(nome, efeito, necessidades, dias_para_colher);
                        if (nova != NULL) {
                            enqueue(&fila, nova); // Usar a função enqueue
                            printf("\n\n🌱 Planta '%s' adicionada à fila com sucesso!\n", nome);
                            printf("✨ Efeito: %s\n", efeito);
                            printf("🌿 Necessidades: %s\n", necessidades);
                            printf("📅 Tempo para colher: %d dias\n", dias_para_colher);
                        } else {
                            printf("❌ Erro ao criar a planta.\n");
                        }
                    } else {
                        printf("❌ Erro: Resposta da API inválida.\n");
                    }
                } else {
                    printf("❌ Erro ao consultar a API para gerar uma planta.\n");
                }
                break;
            }
            case 2:
                ordenarPlantasPorColheita(&fila); // Ordena as plantas antes de listar
                listarPlantas(fila);
                break;
            case 3:
                avancarDia(&fila);
                dias_restantes--;
                if (dias_restantes == 0) {
                    printf("⏳ O tempo acabou!\n");
                    if (verificarObjetivo(&fila, objetivo_atual)) {
                        printf("🎉 Parabéns! Você cumpriu o objetivo da missão!\n");
                    } else {
                        printf("❌ Você não conseguiu cumprir o objetivo da missão.\n");
                    }
                    opcao = 0; 
                }
                break;
            case 4: {
                Planta* colhida = dequeue(&fila); // Usar a função dequeue
                if (colhida != NULL) {
                    enqueue(&colhidas, colhida); // Adicionar à fila de colhidas
                    printf("\n\n🌾 Planta '%s' foi colhida com sucesso!\n", colhida->nome);
                } else {
                    printf("❌ Não há plantas para colher.\n");
                }
                break;
            }
            case 5:
                printf("\n\n📜 Enigma da Missão: '%s'\n\n", objetivo_atual->descricao);
                printf("🎯 Objetivo da Missão: Colher plantas com o efeito de %s e de %s! \n", objetivo_atual->nome1, objetivo_atual->nome2);
                break;
            case 6:
                checarPlantasColhidas(&colhidas);
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