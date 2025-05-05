#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "../include/api.h"

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

int consultar_api(const char *api_key, const char *prompt_text, char *resposta, size_t tamanho_resposta) {
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
             model, api_key);

    char json_payload[1024];
    snprintf(json_payload, sizeof(json_payload),
             "{\"contents\":[{\"parts\":[{\"text\":\"%s\"}]}]}",
             prompt_text);

    curl_global_init(CURL_GLOBAL_ALL);
    curl_handle = curl_easy_init();

    if (!curl_handle) {
        fprintf(stderr, "Erro ao inicializar o handle da libcurl.\n");
        free(chunk.buffer);
        curl_global_cleanup();
        return 1;
    }

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json; charset=UTF-8");

    curl_easy_setopt(curl_handle, CURLOPT_URL, api_url);
    curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, json_payload);
    curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

    res = curl_easy_perform(curl_handle);

    long http_code = 0;
    curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);

    if (res != CURLE_OK || http_code != 200) {
        fprintf(stderr, "Erro na requisição: %s\n", curl_easy_strerror(res));
        free(chunk.buffer);
        curl_easy_cleanup(curl_handle);
        curl_slist_free_all(headers);
        curl_global_cleanup();
        return 1;
    }

    // Copia a resposta para o buffer fornecido pelo chamador
    strncpy(resposta, chunk.buffer, tamanho_resposta - 1);
    resposta[tamanho_resposta - 1] = '\0';  // Garante terminação nula

    // Limpeza
    free(chunk.buffer);
    curl_easy_cleanup(curl_handle);
    curl_slist_free_all(headers);
    curl_global_cleanup();

    return 0;
}
