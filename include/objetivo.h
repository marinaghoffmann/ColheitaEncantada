#ifndef OBJETIVO_H
#define OBJETIVO_H

typedef struct {
    const char *nome1;
    const char *nome2;
    const char *descricao;
} Objetivo;

extern const Objetivo objetivos[];

#define NUM_OBJETIVOS 10

#endif