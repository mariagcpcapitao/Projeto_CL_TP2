

#ifndef UTILS_H
#define UTILS_H
#include "algoritmo.h"
#include <stdlib.h>
//geral
void init_rand();
void free_info(struct info x);
struct info init_data(char *s);
int random_l_h(int min, int max);
float rand_01();
void escreve_sol(int *sol,struct info x);
void substitui(int a[], int b[], int n);


//local
void gera_sol_inicial(int *sol, struct info x);





//evolutivo
struct info init_data(char *filename);
pchrom init_pop(struct info d);
void free_pop(pchrom pop, int popsize);
void free_data(struct info d);

int flip();

chrom get_best(pchrom pop, struct info d, chrom best);

#endif //UTILS_H
