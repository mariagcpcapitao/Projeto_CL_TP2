#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "algoritmo.h"
#include "utils.h"
#include "funcao.h"

// Inicializa��o do gerador de n�meros aleat�rios
void init_rand()
{
	srand((unsigned)time(NULL));
}
//alocar memoria
double** create_distance_matrix(int n) {

    double** matrix = malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        matrix[i] = (double*)calloc(n, sizeof(double));
    }
    return matrix;
}
//libertar memoria
void free_distance_matrix(double** matrix, int n) {
    for (int i = 0; i < n; i++) {
        free(matrix[i]);
    }
    free(matrix);
}


void free_info(struct info x) {
    if (x.distancias != NULL) {
        free_distance_matrix(x.distancias, x.numlocais);
        x.distancias = NULL;
    }
}

// Leitura dos par�metros e dos dados do problema
// Par�metros de entrada: Nome do ficheiro e matriz a preencher com os dados dos objectos (peso e valor)
// Par�metros de sa�da: Devolve a estrutura com os par�metros
struct info init_data(char *filename) {
	struct  info x;
	FILE    *f;
	int     i,j;
	char node1[10], node2[10];
	double dist;

	f = fopen(filename, "rt");
	if (!f)
	{
		printf("File not found\n");
		exit(1);
	}
	// Numero de candidaturas e  Numero de locais
	fscanf(f, "%d %d", &x.numcand, &x.numlocais);

	// Leitura dos parametros do problema
	x.distancias = create_distance_matrix(x.numcand);


	// Define valores padrao para parametros do EA (alterar na parte de teste para ver com que parametros obtemos melhores resultados)
	x.popsize = 100;
	x.pm = 0.001;      // Probabilidade baixa para mutacao
	x.pr = 0.7;       // Probabilidade alta para crossover
	x.tsize = 2;
	x.numGenerations = 2500;

	// Lê as distâncias

	//preencher a diagonal (distancia do ponto a ele mm)
	for(i = 0; i < x.numcand; i++)
		x.distancias[i][i] = 0.0;

	while (fscanf(f, "%s %s %lf", node1, node2, &dist) == 3) {
		// Converte "eX" para índice
		i = atoi(node1+1) - 1;
		j = atoi(node2+1) - 1;

		if (i < 0 || i >= x.numcand || j < 0 || j >= x.numcand) {
			printf("Erro: índice inválido %d %d\n", i, j);
			continue;
		}
		x.distancias[i][j] = dist;
		x.distancias[j][i] = dist;// grafo não direcionado

    }

    fclose(f);
    return x;
}

void gera_sol_inicial(int *sol, struct info x) {
	int i, j;
	int count = 0;

	// Inicializa tudo com 0
	for (i = 0; i < x.numcand; i++) {
		sol[i] = 0;
	}
	//sempre soluções validas
	while (count < x.numlocais) {
		j = random_l_h(0, x.numcand - 1);
		if (sol[j] == 0) {
			sol[j] = 1;
			count++;
		}
	}
}
void escreve_sol(int *sol ,struct info x) {
	int i;
	printf("\nCusto: %.6f | Locais: ", calcula_custo(sol,x));
	for (i=0;i<x.numcand;i++) {
		if (sol[i] == 1) {
			printf("%d  ", i+1);
		}
	}
	printf("\n");
}
// copia vetor b para a (tamanho n)
void substitui(int a[], int b[], int n)
{
	int i;
	for(i=0; i<n; i++)
		a[i]=b[i];
}
// Devolve um valor inteiro distribuido uniformemente entre min e max
int random_l_h(int min, int max)
{
	return min + rand() % (max-min+1);
}

// Devolve um valor real distribuido uniformemente entre 0 e 1
float rand_01()
{
	return ((float)rand())/RAND_MAX;
}


// Simula o lan�amento de uma moeda, retornando o valor 0 ou 1

int flip()
{
	if ((((float)rand()) / RAND_MAX) < 0.5)
		return 0;

	return 1;
}
pchrom init_pop(struct info d) {
	int i, r, count;
	pchrom indiv = malloc(sizeof(chrom) * d.popsize);
	if (!indiv) exit(1);

	for (i = 0; i < d.popsize; i++) {
		indiv[i].p = calloc(d.numcand, sizeof(int));
		count = 0;
		// Gera solucao valida com exatamente m locais
		while(count < d.numlocais) {
			r = random_l_h(0, d.numcand - 1);
			if(indiv[i].p[r] == 0) {
				indiv[i].p[r] = 1;
				count++;
			}
		}
		indiv[i].valido = 1;
	}
	return indiv;
}

void free_pop(pchrom pop, int popsize) {
	if(!pop) return;
	for(int i=0; i<popsize; i++) free(pop[i].p);
	free(pop);
}

void free_data(struct info d) {
	for(int i=0; i<d.numcand; i++) free(d.distancias[i]);
	free(d.distancias);
}

chrom get_best(pchrom pop, struct info d, chrom best) {
	for (int i = 0; i < d.popsize; i++) {
		if (pop[i].fitness > best.fitness)
			best = pop[i];
	}
	return best;
}

