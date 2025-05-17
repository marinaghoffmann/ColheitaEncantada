

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

#include <stdbool.h>
#define Rectangle RayRectangle
#define CloseWindow RayCloseWindow
#define ShowCursor RayShowCursor
#undef LoadImage
#define LoadImage RayLoadImage
#undef DrawText
#undef DrawTextEx
#define DrawTextEx RayDrawTextEx
#undef PlaySound
#define PlaySound RayPlaySound


#include "../raylib/include/raylib.h"

typedef struct Botao {
    Rectangle rect;
    Color color;
}Botao;

void plantarSemente(Fila *fila, char* texto1, char* texto2, char* texto3, char* texto4, bool* mostrar, double* tempoInicio);
void processarAvancoDeDia(Fila *fila, const Objetivo *objetivo_atual, int *dias_restantes, int *opcao);
void criarBotao (Botao *butao, Rectangle rect, Color color);
void desenharBotao(Botao botao) {
    DrawRectangleRec(botao.rect, botao.color);
}




int main() {
    Fila fila;
    inicializarFila(&fila);
    Fila colhidas;
    inicializarFila(&colhidas);
    int opcao;
    srand(time(NULL)); 
    int indice_objetivo = rand() % NUM_OBJETIVOS;
    const Objetivo *objetivo_atual = &objetivos[indice_objetivo];
    int dias_restantes = 10;
    char textoPlantaSemente1[256], textoPlantaSemente2[256], textoPlantaSemente3[256], textoPlantaSemente4[256];
    bool mostrarMensagemPlantar = false;
    double tempoMensagemPlantar = 0;


    const int screenWidth = 1600;
    const int screenHeight = 900;
    Botao botaoPlantarSemente;
    Botao botaoVisualizarPlantas;
    Botao botaoAvancarDia;
    Botao botaoColherPlantas;

    double tempoInicio = 0;
    bool mostrarMensagem = false;

    InitWindow(screenWidth, screenHeight, "Colheita Mágica");

    criarBotao(&botaoPlantarSemente, (Rectangle){ screenWidth/2 - 110, screenHeight/2 - 50, 400, 100 }, RED);
    criarBotao(&botaoVisualizarPlantas, (Rectangle){ screenWidth/2 - 110, screenHeight/2 - 200, 400, 100 }, RED);
    criarBotao(&botaoAvancarDia, (Rectangle){ screenWidth/2 - 110, screenHeight/2 - 350, 400, 100 }, RED);
    criarBotao(&botaoColherPlantas, (Rectangle){ screenWidth/2 - 110, screenHeight/2 - 500, 400, 100 }, RED);


    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();

        Vector2 mouse = GetMousePosition();
        char texto1[500], texto2[500], texto3[500];

        sprintf(texto1,"\n📅 Dias Restantes Na Missão: %d dias\n", dias_restantes);
        DrawText(texto1, 10 , 0, 20, BLACK);
        sprintf(texto2,"\n\n📜 Enigma da Missão: '%s'\n\n", objetivo_atual->descricao);
        DrawText(texto2, 10, 50 ,12, BLACK);
        sprintf(texto3,"🎯 Objetivo da Missão: Colher plantas com o efeito de %s e de %s! \n", objetivo_atual->nome1, objetivo_atual->nome2);
        DrawText(texto3, 10, 100,12, BLACK);

        desenharBotao(botaoPlantarSemente);
        DrawText("Plantar uma semente!", screenWidth/2 - 110, screenHeight/2 - 50, 20, BLACK);

        desenharBotao(botaoVisualizarPlantas);
        DrawText("Visualizar Plantas", screenWidth/2 - 110, screenHeight/2 - 200, 20, BLACK);

        desenharBotao(botaoAvancarDia);
        DrawText("Avançar o dia", screenWidth/2 - 110, screenHeight/2 - 350, 20, BLACK);

        desenharBotao(botaoColherPlantas);
        DrawText("Colher plantas", screenWidth/2 - 110, screenHeight/2 - 500, 20, BLACK);


        if (CheckCollisionPointRec(mouse, botaoPlantarSemente.rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            plantarSemente(&fila, textoPlantaSemente1, textoPlantaSemente2, textoPlantaSemente3, textoPlantaSemente4, &mostrarMensagemPlantar, &tempoMensagemPlantar);
        }
        if (CheckCollisionPointRec(mouse, botaoVisualizarPlantas.rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            //listarPlantas(fila);
        }

        if (mostrarMensagemPlantar && GetTime() - tempoMensagemPlantar < 10.0) {
            DrawText(textoPlantaSemente1, 50, 200, 20, DARKGREEN);
            DrawText(textoPlantaSemente2, 50, 230, 20, DARKGREEN);
            DrawText(textoPlantaSemente3, 50, 260, 20, DARKGREEN);
            DrawText(textoPlantaSemente4, 50, 290, 20, DARKGREEN);
        } else {
            mostrarMensagemPlantar = false;
        }

        ClearBackground(RAYWHITE);

        EndDrawing();
    }



    do {
        sprintf("\n📅 Dias Restantes Na Missão: %d dias\n", dias_restantes);

        menu();
        scanf("%d", &opcao);
        getchar();

        switch (opcao) {
            case 1: {// FEITO
                plantarSemente(&fila, textoPlantaSemente1, textoPlantaSemente2, textoPlantaSemente3, textoPlantaSemente4, &mostrarMensagemPlantar, &tempoMensagemPlantar);
                break;
            }
            case 2:
                listarPlantas(fila);
                break;
            case 3:
                processarAvancoDeDia(&fila, objetivo_atual, &dias_restantes, &opcao);
                break;
            case 4:
                colher(&fila);
                break;
            case 5:
                printf("\n\n📜 Enigma da Missão: '%s'\n\n", objetivo_atual->descricao);
                printf("🎯 Objetivo da Missão: Colher plantas com o efeito de %s e de %s! \n", objetivo_atual->nome1, objetivo_atual->nome2);
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


void plantarSemente(Fila *fila, char* texto1, char* texto2, char* texto3, char* texto4, bool* mostrar, double* tempoInicio) {
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
        char nome[100], descricao_efeito[300], necessidades[300];
        if (sscanf(resposta, "%99[^|]|%299[^|]|%299[^\n]", nome, descricao_efeito, necessidades) == 3) {
            int dias_para_colher = 9 + rand() % 4;
            Planta* nova = criarPlanta(nome, descricao_efeito, necessidades, dias_para_colher);
            if (nova != NULL) {
                if (fila->fim == NULL) {
                    fila->inicio = fila->fim = nova;
                } else {
                    fila->fim->prox = nova;
                    fila->fim = nova;
                }

                // Prepara os textos e inicia o temporizador
                *mostrar = true;
                *tempoInicio = GetTime();
                sprintf(texto1, "🌱 Planta '%s' adicionada à fila com sucesso!", nome);
                sprintf(texto2, "✨ Efeito: %s", descricao_efeito);
                sprintf(texto3, "🌿 Necessidades: %s", necessidades);
                sprintf(texto4, "📅 Tempo para colher: %d dias", dias_para_colher);
            }
        }
    }
}

void processarAvancoDeDia(Fila *fila, const Objetivo *objetivo_atual, int *dias_restantes, int *opcao) {
    avancarDia(fila);
    (*dias_restantes)--;

    if (*dias_restantes == 0) {
        printf("⏳ O tempo acabou!\n");

        if (verificarObjetivo(fila, objetivo_atual)) {
            printf("🎉 Parabéns! Você cumpriu o objetivo da missão!\n");
        } else {
            printf("❌ Você não conseguiu cumprir o objetivo da missão.\n");
        }

        *opcao = 0; // encerra o jogo no switch
    }
}

void criarBotao (Botao *butao, Rectangle rect, Color color) {
    butao->rect = rect;
    butao->color = color;
}