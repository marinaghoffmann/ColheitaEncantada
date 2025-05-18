#include "../include/planta.h"
#include "../include/jogo.h"
#include "../include/menu.h"
#include "../include/api.h"
#include "../include/objetivo.h"
#include "../include/item_magico.h"

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

const ItemMagico itens_magicos[NUM_ITENS_MAGICOS] = {
    {"Luz de Umbra", "Uma sombra que dança entre a luz e a escuridão, trazendo mistério e dúvida.", 1, 1},
    {"Chuva de Éter", "Gotas mágicas que caem do céu, acelerando o crescimento das plantas.", 1, 2},
    {"Raio de Sol", "Um feixe dourado que aquece a alma, mas pode queimar o que toca.", -1, 1},
    {"Orvalho Lunar", "Uma névoa prateada que nutre as plantas, mas pode trazer frio indesejado.", 1, 1},
    {"Adubo Estelar", "Poeira cósmica que enriquece o solo, mas pode sobrecarregar as raízes.", -1, 2}
};

void plantarSemente(Fila *fila, char* texto1, char* texto2, char* texto3, char* texto4, bool* mostrar, double* tempoInicio);
void processarAvancoDeDia(Fila *fila, const Objetivo *objetivo_atual, int *dias_restantes, int *opcao,
                          char *mensagem1, char *mensagem2, bool *mostrarMensagem, double *tempoMensagem);
void criarBotao (Botao *butao, Rectangle rect, Color color);
void desenharBotao(Botao botao) {
    DrawRectangleRec(botao.rect, botao.color);
}
bool verificarCliqueBotao(Botao botao, Vector2 mouse);
void listarPlantas_raylib(Fila fila, int x, int y_inicial);
void avancarDia(Fila *fila);
void usarItemMagico(Fila *fila);
void colher(Fila* fila, char* texto1, char* texto2, bool* mostrar, double* tempoInicio);




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



    const int screenWidth = 1600;
    const int screenHeight = 900;
    Botao botaoPlantarSemente;
    Botao botaoVisualizarPlantas;
    Botao botaoAvancarDia;
    Botao botaoColherPlantas;

    bool mostrarMensagemPlantar = false;
    double tempoMensagemPlantar = 0;
    char textoColheita1[256], textoColheita2[256];
    bool mostrarMensagemColheita = false;
    double tempoMensagemColheita = 0;


    bool mostrarLista = false;
    double tempoLista = 0;

    // variaveis para função avancadia
    char msgFimDia1[256], msgFimDia2[256];
    bool mostrarFimDia = false;
    double tempoFimDia = 0;

    InitWindow(screenWidth, screenHeight, "Colheita Mágica");

    criarBotao(&botaoPlantarSemente, (Rectangle){ 10, 120, 200, 50 }, RED);
    criarBotao(&botaoVisualizarPlantas, (Rectangle){ 10, 180, 200, 50 }, RED);
    criarBotao(&botaoAvancarDia, (Rectangle){ 10, 240, 200, 50 }, RED);
    criarBotao(&botaoColherPlantas, (Rectangle){ 10, 300, 200, 50 }, RED);


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
        DrawText("Plantar uma semente!", 10, 120, 20, BLACK);

        desenharBotao(botaoVisualizarPlantas);
        DrawText("Visualizar Plantas", 10, 180, 20, BLACK);

        desenharBotao(botaoAvancarDia);
        DrawText("Avançar o dia", 10, 240, 20, BLACK);

        desenharBotao(botaoColherPlantas);
        DrawText("Colher plantas", 10, 300, 20, BLACK);


        if (verificarCliqueBotao(botaoPlantarSemente, mouse)) {
            plantarSemente(&fila, textoPlantaSemente1, textoPlantaSemente2, textoPlantaSemente3, textoPlantaSemente4, &mostrarMensagemPlantar, &tempoMensagemPlantar);
        }

        if (verificarCliqueBotao(botaoVisualizarPlantas, mouse)) {
            mostrarLista = true;
            tempoLista = GetTime();
        }

        if (verificarCliqueBotao(botaoAvancarDia, mouse)) {
            processarAvancoDeDia(&fila, objetivo_atual, &dias_restantes, &opcao, msgFimDia1, msgFimDia2, &mostrarFimDia, &tempoFimDia);
        }

        if (verificarCliqueBotao(botaoColherPlantas, mouse)) {
            colher(&fila, textoColheita1, textoColheita2, &mostrarMensagemColheita, &tempoMensagemColheita);
        }


        if (mostrarMensagemPlantar && GetTime() - tempoMensagemPlantar < 30.0) {
            DrawText(textoPlantaSemente1, 300, 200, 20, DARKGREEN);
            DrawText(textoPlantaSemente2, 300, 230, 20, DARKGREEN);
            DrawText(textoPlantaSemente3, 300, 260, 20, DARKGREEN);
            DrawText(textoPlantaSemente4, 300, 290, 20, DARKGREEN);
        } else {
            mostrarMensagemPlantar = false;
        }



        ClearBackground(RAYWHITE);

        EndDrawing();
    }

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

void avancarDia(Fila *fila) {
    usarItemMagico(fila);

    Planta *atual = fila->inicio;
    while (atual != NULL) {
        if (atual->dias_para_colher > 0) {
            atual->dias_para_colher--;
        }
        atual = atual->prox;
    }
}


void processarAvancoDeDia(Fila *fila, const Objetivo *objetivo_atual, int *dias_restantes, int *opcao,
                          char *mensagem1, char *mensagem2, bool *mostrarMensagem, double *tempoMensagem) {
    avancarDia(fila);
    (*dias_restantes)--;

    if (*dias_restantes == 0) {
        *opcao = 0;
        *mostrarMensagem = true;
        *tempoMensagem = GetTime();

        sprintf(mensagem1, "⏳ O tempo acabou!");

        if (verificarObjetivo(fila, objetivo_atual)) {
            sprintf(mensagem2, "🎉 Parabéns! Você cumpriu o objetivo da missão!");
        } else {
            sprintf(mensagem2, "❌ Você não conseguiu cumprir o objetivo da missão.");
        }
    }
}

void usarItemMagico(Fila *fila) {
    srand(time(NULL));
    int indice_item = rand() % NUM_ITENS_MAGICOS;
    const ItemMagico *item = &itens_magicos[indice_item];

    bool esperandoEscolha = true;
    bool usouItem = false;

    while (esperandoEscolha) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawText("✨ Item mágico disponível:", 50, 50, 20, PURPLE);
        DrawText(item->nome, 300, 50, 20, DARKPURPLE);

        DrawText("📜 Descrição:", 50, 90, 20, PURPLE);
        DrawText(item->descricao, 200, 90, 20, DARKGRAY);

        DrawText("Deseja usar este item? [S]im / [N]ão", 50, 140, 20, DARKBLUE);

        EndDrawing();

        if (IsKeyPressed(KEY_S)) {
            usouItem = true;
            esperandoEscolha = false;
        } else if (IsKeyPressed(KEY_N)) {
            usouItem = false;
            esperandoEscolha = false;
        }
    }
}

void criarBotao (Botao *butao, Rectangle rect, Color color) {
    butao->rect = rect;
    butao->color = color;
}

bool verificarCliqueBotao(Botao botao, Vector2 mouse) {
    return CheckCollisionPointRec(mouse, botao.rect) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void listarPlantas_raylib(Fila fila, int x, int y_inicial) {
    ordenarPlantasPorColheita(&fila);

    Planta* atual = fila.inicio;
    int y = y_inicial;
    int espacamento = 80; // altura entre cada planta

    if (atual == NULL) {
        DrawText("🌱 Nenhuma planta na fila.", x, y, 20, DARKGRAY);
    } else {
        while (atual != NULL) {
            char texto1[256], texto2[256], texto3[256], texto4[256];

            sprintf(texto1, "🌿 Nome: %s", atual->nome);
            sprintf(texto2, "💌 Efeito: %s", atual->efeito);
            sprintf(texto3, "✨ Necessidades: %s", atual->necessidades);
            sprintf(texto4, "📅 Dias para colher: %d", atual->dias_para_colher);

            DrawText(texto1, x, y, 20, DARKGREEN);       y += 20;
            DrawText(texto2, x, y, 18, BLACK);           y += 20;
            DrawText(texto3, x, y, 18, BLACK);           y += 20;
            DrawText(texto4, x, y, 18, BLACK);           y += espacamento;

            atual = atual->prox;
        }
    }
}

void colher(Fila* fila, char* texto1, char* texto2, bool* mostrar, double* tempoInicio) {
    int colheuAlguma = 0;

    if (fila->inicio != NULL && fila->inicio->dias_para_colher == 0) {
        Planta* colhida = fila->inicio;
        sprintf(texto1, "🧺 Colhendo planta: %s", colhida->nome);
        fila->inicio = fila->inicio->prox;
        free(colhida);
        colheuAlguma = 1;
    }

    if (fila->inicio == NULL) {
        fila->fim = NULL;
    }

    if (!colheuAlguma) {
        if (fila->inicio != NULL) {
            sprintf(texto1, "⏳ Próxima planta '%s' ainda não está pronta.", fila->inicio->nome);
            sprintf(texto2, "⏳ Faltam %d dias para colher.", fila->inicio->dias_para_colher);
        } else {
            sprintf(texto1, "🌾 Nenhuma planta disponível para colheita no momento.");
            texto2[0] = '\0';
        }
    } else {
        texto2[0] = '\0';
    }

    *mostrar = true;
    *tempoInicio = GetTime();
}
