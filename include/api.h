// api.h

#ifndef API_H
#define API_H

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

// Declaração da função
int consultar_api(const char *api_key, const char *prompt_text, char *resposta, size_t tamanho_resposta);

Planta* criarPlanta(const char *nome, const char *efeito, int dias, int valor);

#endif 
