#ifndef ALGORITMO_H
#define ALGORITMO_H
#define MAX_OBJ 1000		// Numero maximo de objectos

#include <stdlib.h>
// Estruturas para armazenar parametros
struct info
{
    //Num de candidaturas
    int     numcand;

    //Num de locais
    int   numlocais;

    // Distancias entre locais
    double **distancias;

    int     popsize;        // Tamanho da populacao
    float   pm;             // Probabilidade de mutacao
    float   pr;             // Probabilidade de recombinacao
    int     tsize;          // Tamanho do torneio
    int     numGenerations; // Numero de geracoes

};

struct individual
{
    int     *p;      // Solucao (vetor binario de tamanho numcand)
    double  fitness; // Valor da qualidade (Distancia Media)
    int     valido;  // 1 se valido (tem m locais), 0 senao
};

// Individuo (solu��o)
typedef struct individual chrom, *pchrom;

//pesquisa local
void gera_vizinho_simples(int a[], int b[], int n);
void gera_vizinho_duplo(int a[], int b[], int n);
double trepa_colinas(int sol[], struct info x, int itera);

//evolutivo
void tournament(pchrom pop, struct info d, pchrom parents);
void roulette(pchrom pop, struct info d, pchrom parents);
void genetic_operators(pchrom parents, struct info d, pchrom offspring);
#endif //ALGORITMO_H
