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

// Função que consulta a API Gemini e retorna a resposta
int consultar_api(const char *api_key, const char *prompt_text) {
    CURL *curl_handle;
    CURLcode res;
    MemoryStruct chunk;

    // Inicializa o buffer de memória para a resposta
    chunk.buffer = malloc(1); // Começa com 1 byte (para o terminador nulo inicial)
    if (chunk.buffer == NULL) {
        fprintf(stderr, "Erro: Falha ao alocar memória inicial.\n");
        return 1;
    }
    chunk.size = 0;    // Nenhum dado ainda

    // Defina o modelo e o endpoint
    const char *model = "gemini-1.5-flash-latest"; // Ou "gemini-pro", etc.
    char api_url[512];
    snprintf(api_url, sizeof(api_url),
             "https://generativelanguage.googleapis.com/v1beta/models/%s:generateContent?key=%s",
             model, api_key);

    // Monte o corpo da requisição JSON
    char json_payload[1024];
    snprintf(json_payload, sizeof(json_payload),
             "{\"contents\":[{\"parts\":[{\"text\":\"%s\"}]}]}",
             prompt_text);

    // Inicialização global da libcurl
    curl_global_init(CURL_GLOBAL_ALL);

    // Inicializa o handle easy
    curl_handle = curl_easy_init();

    if (curl_handle) {
        // Configura a URL da API (já inclui a API Key)
        curl_easy_setopt(curl_handle, CURLOPT_URL, api_url);

        // Configura para ser uma requisição POST
        curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);

        // Define o corpo (payload) da requisição POST (nosso JSON)
        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, json_payload);

        // Define o cabeçalho Content-Type
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json; charset=UTF-8");
        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, headers);

        // Define a função de callback para receber a resposta
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

        // Passa nossa estrutura 'chunk' para a função de callback
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&chunk);

        // Executa a requisição
        res = curl_easy_perform(curl_handle);

        // Verifica erros na execução
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() falhou: %s\n", curl_easy_strerror(res));
        } else {
            // Requisição bem-sucedida
            long http_code = 0;
            curl_easy_getinfo(curl_handle, CURLINFO_RESPONSE_CODE, &http_code);
            printf("Código de Status HTTP: %ld\n", http_code);

            // Exibe a resposta completa
            printf("Resposta JSON completa:\n%s\n", chunk.buffer);
        }

        // Limpeza
        curl_easy_cleanup(curl_handle);
        free(chunk.buffer); // Libera a memória alocada para a resposta
        curl_slist_free_all(headers); // Libera a lista de cabeçalhos
    } else {
        fprintf(stderr, "Erro ao inicializar o handle da libcurl.\n");
    }

    // Limpeza global da libcurl
    curl_global_cleanup();

    return 0;
}
