#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "../include/api.h"
#include <cjson/cJSON.h>
#define API_KEY "AIzaSyDZ6Ch6dNIdxqGAHjKKurJqrxYpRB1DZlg"

// Estrutura para armazenar a resposta da API
typedef struct {
    char *buffer;
    size_t size;
} MemoryStruct;

// Função de callback para escrever os dados recebidos na memória
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    MemoryStruct *mem = (MemoryStruct *)userp;

    // Realoca o buffer para acomodar os novos dados + terminador nulo
    char *ptr = realloc(mem->buffer, mem->size + realsize + 1);
    if (ptr == NULL) {
        fprintf(stderr, "Erro: falha ao alocar memória no callback!\n");
        return 0; // Retornar 0 sinaliza erro para libcurl
    }

    mem->buffer = ptr;
    memcpy(&(mem->buffer[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->buffer[mem->size] = 0; // Adiciona terminador nulo

    return realsize;
}

// Função para escapar caracteres especiais no JSON
void escapar_json(const char *origem, char *destino, size_t tamanho_destino) {
    size_t i = 0, j = 0;
    while (origem[i] != '\0' && j < tamanho_destino - 1) {
        if (origem[i] == '\"' || origem[i] == '\\') {
            if (j + 1 < tamanho_destino - 1) {
                destino[j++] = '\\';
            } else {
                break;
            }
        }
        destino[j++] = origem[i++];
    }
    destino[j] = '\0';
}

// Função para remover delimitadores Markdown
void limpar_delimitadores_markdown(const char *entrada, char *saida, size_t tamanho_saida) {
    const char *inicio = strstr(entrada, "```json\n");
    if (inicio) {
        inicio += strlen("```json\n"); // Pula o delimitador inicial
        const char *fim = strstr(inicio, "```");
        if (fim) {
            size_t tamanho = fim - inicio;
            if (tamanho < tamanho_saida - 1) {
                strncpy(saida, inicio, tamanho);
                saida[tamanho] = '\0';
                return;
            }
        }
    }
    // Se não encontrar delimitadores, copia a entrada original
    strncpy(saida, entrada, tamanho_saida - 1);
    saida[tamanho_saida - 1] = '\0';
}

int consultar_api(const char *prompt_text, char *resposta, size_t tamanho_resposta) {
    CURL *curl_handle;
    CURLcode res;
    MemoryStruct chunk;

    chunk.buffer = malloc(1);  // Inicialmente vazio
    if (chunk.buffer == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória inicial.\n");
        return 1;
    }
    chunk.size = 0;

    const char *model = "gemini-1.5-flash-latest";
    char api_url[512];
    snprintf(api_url, sizeof(api_url),
             "https://generativelanguage.googleapis.com/v1beta/models/%s:generateContent?key=%s",
             model, API_KEY);

    // Escapar o prompt_text
    char prompt_escapado[1024];
    escapar_json(prompt_text, prompt_escapado, sizeof(prompt_escapado));

    char json_payload[2048];
    snprintf(json_payload, sizeof(json_payload),
             "{"
             "  \"contents\": ["
             "    {"
             "      \"role\": \"user\","
             "      \"parts\": ["
             "        { \"text\": \"%s\" }"
             "      ]"
             "    }"
             "  ]"
             "}", prompt_escapado);

    // Configurar a requisição CURL
    curl_handle = curl_easy_init();
    if (!curl_handle) {
        fprintf(stderr, "Erro ao inicializar o CURL.\n");
        free(chunk.buffer);
        return 1;
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    curl_easy_setopt(curl_handle, CURLOPT_URL, api_url);
    curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl_handle);

    if (res != CURLE_OK) {
        fprintf(stderr, "Erro na requisição: %s\n", curl_easy_strerror(res));
        free(chunk.buffer);
        curl_easy_cleanup(curl_handle);
        curl_slist_free_all(headers);
        return 1;
    }

    // guardando pra caso a api dê bronca de novo: tirar do comentário pra ver resposta exata da api
    // printf("Resposta da API: %s\n", chunk.buffer);

    cJSON *json = cJSON_Parse(chunk.buffer);
    if (!json) {
        fprintf(stderr, "Erro ao analisar o JSON retornado pela API.\n");
        free(chunk.buffer);
        curl_easy_cleanup(curl_handle);
        curl_slist_free_all(headers);
        return 1;
    }

    cJSON *candidates = cJSON_GetObjectItemCaseSensitive(json, "candidates");
    if (!cJSON_IsArray(candidates)) {
        fprintf(stderr, "Erro: Campo 'candidates' não encontrado ou não é um array.\n");
        cJSON_Delete(json);
        free(chunk.buffer);
        curl_easy_cleanup(curl_handle);
        curl_slist_free_all(headers);
        return 1;
    }

    cJSON *first_candidate = cJSON_GetArrayItem(candidates, 0);
    if (!first_candidate) {
        fprintf(stderr, "Erro: Primeiro candidato não encontrado.\n");
        cJSON_Delete(json);
        free(chunk.buffer);
        curl_easy_cleanup(curl_handle);
        curl_slist_free_all(headers);
        return 1;
    }

    cJSON *content = cJSON_GetObjectItemCaseSensitive(first_candidate, "content");
    if (!content) {
        fprintf(stderr, "Erro: Campo 'content' não encontrado.\n");
        cJSON_Delete(json);
        free(chunk.buffer);
        curl_easy_cleanup(curl_handle);
        curl_slist_free_all(headers);
        return 1;
    }

    cJSON *parts = cJSON_GetObjectItemCaseSensitive(content, "parts");
    if (!cJSON_IsArray(parts)) {
        fprintf(stderr, "Erro: Campo 'parts' não encontrado ou não é um array.\n");
        cJSON_Delete(json);
        free(chunk.buffer);
        curl_easy_cleanup(curl_handle);
        curl_slist_free_all(headers);
        return 1;
    }

    cJSON *first_part = cJSON_GetArrayItem(parts, 0);
    if (!first_part) {
        fprintf(stderr, "Erro: Primeiro elemento de 'parts' não encontrado.\n");
        cJSON_Delete(json);
        free(chunk.buffer);
        curl_easy_cleanup(curl_handle);
        curl_slist_free_all(headers);
        return 1;
    }

    cJSON *text = cJSON_GetObjectItemCaseSensitive(first_part, "text");
    if (!cJSON_IsString(text)) {
        fprintf(stderr, "Erro: Campo 'text' não encontrado ou não é uma string.\n");
        cJSON_Delete(json);
        free(chunk.buffer);
        curl_easy_cleanup(curl_handle);
        curl_slist_free_all(headers);
        return 1;
    }

    char json_limpo[2048];
    limpar_delimitadores_markdown(text->valuestring, json_limpo, sizeof(json_limpo));

    cJSON *inner_json = cJSON_Parse(json_limpo);
    if (!inner_json) {
        fprintf(stderr, "Erro ao analisar o JSON interno em 'text'.\n");
        cJSON_Delete(json);
        free(chunk.buffer);
        curl_easy_cleanup(curl_handle);
        curl_slist_free_all(headers);
        return 1;
    }

    cJSON *nome = cJSON_GetObjectItemCaseSensitive(inner_json, "nome");
    cJSON *efeito = cJSON_GetObjectItemCaseSensitive(inner_json, "efeito");
    cJSON *necessidades = cJSON_GetObjectItemCaseSensitive(inner_json, "necessidades");

    if (cJSON_IsString(nome) && cJSON_IsString(efeito) && cJSON_IsString(necessidades)) {
        snprintf(resposta, tamanho_resposta, "%s|%s|%s", nome->valuestring, efeito->valuestring, necessidades->valuestring);
    } else {
        fprintf(stderr, "Erro: JSON interno não contém os campos esperados.\n");
        cJSON_Delete(inner_json);
        cJSON_Delete(json);
        free(chunk.buffer);
        curl_easy_cleanup(curl_handle);
        curl_slist_free_all(headers);
        return 1;
    }

    cJSON_Delete(inner_json);
    cJSON_Delete(json);
    free(chunk.buffer);
    curl_easy_cleanup(curl_handle);
    curl_slist_free_all(headers);

    return 0;
}
