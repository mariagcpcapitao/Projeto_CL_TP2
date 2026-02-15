
#ifndef FUNCAO_H
#define FUNCAO_H
#include "algoritmo.h"
#include "utils.h"
#include <stdio.h>
//geral
double calcula_custo(int sol[], struct info x);


//evol
// Cálculo de custo (distância média)
double calcula_custo(int sol[], struct info x);

// Penalização
double penalizacao(int *sol, struct info x);

// Reparação
void repara(int *sol, struct info d);

// Avaliação da população
void evaluate(pchrom pop, struct info d, int invalid_strategy);

#endif //FUNCAO_H
