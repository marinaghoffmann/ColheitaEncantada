#ifndef ITEM_MAGICO_H
#define ITEM_MAGICO_H

typedef struct {
    const char *nome;       
    const char *descricao;  
    int efeito;             
    int impacto;            
} ItemMagico;

extern const ItemMagico itens_magicos[];

#define NUM_ITENS_MAGICOS 5

#endif