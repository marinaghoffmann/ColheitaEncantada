// api.h

#ifndef API_H
#define API_H

#include <stddef.h> // Adicionado para definir size_t

// Declaração da função
int consultar_api(const char *prompt_text, char *resposta, size_t tamanho_resposta);

#endif
