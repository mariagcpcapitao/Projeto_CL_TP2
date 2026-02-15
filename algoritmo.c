
#include <stdio.h>
#include <stdlib.h>
#include "algoritmo.h"

#include <tgmath.h>

#include "utils.h"
#include "funcao.h"

//local
void gera_vizinho_simples(int a[], int b[], int n) {
    int i, p1, p2;

    for(i=0; i<n; i++) b[i]=a[i];

    // Encontra um 0
    do p1=random_l_h(0, n-1); while(b[p1] != 0);
    // Encontra um 1
    do p2=random_l_h(0, n-1); while(b[p2] != 1);

    // Troca so um par
    b[p1] = 1;
    b[p2] = 0;

}

void gera_vizinho_duplo(int a[], int b[], int n)
{
    int i, p1, p2, p3, p4;

    for(i=0; i<n; i++)
        b[i]=a[i];
    // Encontra posicao com valor 0
    do
        p1=random_l_h(0, n-1);
    while(b[p1] != 0);
    // Encontra posicao com valor 1
    do
        p2=random_l_h(0, n-1);
    while(b[p2] != 1);
    // Encontra posicao com valor 0
    do
        p3=random_l_h(0, n-1);
    while(b[p3] != 0 || p1 == p3);
    // Encontra posicao com valor 1
    do
        p4=random_l_h(0, n-1);
    while(b[p4] != 1 || p4 == p2);

    // troca dois pares
    b[p1] = 1;
    b[p2] = 0;
    b[p3] = 1;
    b[p4] = 0;
}
double trepa_colinas(int sol[], struct info x, int itera)
{
    int *nova_sol, i;
    double custo_viz, custo;
    nova_sol = malloc(sizeof(int)*x.numcand);
    if(nova_sol == NULL)
    {
        printf("Erro na alocacao de memoria");
        exit(1);
    }
    // Avalia solução inicial
    custo = calcula_custo(sol, x);

    for (i=0;i<itera;i++){

           gera_vizinho_simples(sol, nova_sol, x.numcand);
            //gera_vizinho_duplo(sol, nova_sol, x.numcand);

        custo_viz = calcula_custo(nova_sol, x);



        if (custo_viz >= custo) { // Maximização: custo >= custo atual para nao ficar preso nas zonas planas
            substitui(sol, nova_sol, x.numcand);
            custo = custo_viz;
        }

    } // Repete itera vezes
    free(nova_sol);
    return custo;
}

//evolutivo

//SELECAO

void tournament(pchrom pop, struct info d, pchrom parents)
{
    int i, x1, x2, maior;

    // Realiza popsize torneios
    for(i=0; i<d.popsize;i++)
    {
       x1 = random_l_h(0, d.popsize-1);
       do
          x2 = random_l_h(0, d.popsize-1);
       while(x1==x2);

       // Seleciona o vencedor
        if(pop[x1].fitness > pop[x2].fitness)
            maior=x1;
        else maior= x2;

       // Copia os dados do vencedor para os pais
        parents[i].fitness = pop[maior].fitness;
        parents[i].valido = pop[maior].valido;

       // Usa a função substitui para copiar o vetor
       substitui(parents[i].p, pop[maior].p, d.numcand);
    }
}

// Roleta
// Em algoritmo.c

void roulette(pchrom pop, struct info d, pchrom parents) {
    double total_fit = 0.0, r, acc;
    int i, j;
    double min_fit = pop[0].fitness; // Assume o fitness do 1º como o mínimo
    double offset = 0.0;

    // encontra o fitness mais baixo
    for(i = 1; i < d.popsize; i++) {
        if(pop[i].fitness < min_fit) {
            min_fit = pop[i].fitness;
        }
    }

    // Calcula o offset: Garante que todos os fitness são >= 1.0 (se houver negativos)
    if (min_fit < 0) {
        // Se o mínimo for negativo, adiciona o valor absoluto do mínimo + 1.0
        offset = fabs(min_fit) + 1.0;
    }

    // Calcula a soma total dos fitness
    for(i = 0; i < d.popsize; i++) {
        total_fit += pop[i].fitness + offset;
    }

    // seleciona N=popsize pais
    for(i = 0; i < d.popsize; i++) {
        // Gera um ponto aleatório na "roda da roleta"
        r = rand_01() * total_fit;
        acc = 0.0;


        for(j = 0; j < d.popsize; j++) {
            acc += pop[j].fitness + offset; // Usa o fitness transformado

            if(acc >= r) {

                parents[i].fitness = pop[j].fitness;
                parents[i].valido = pop[j].valido;
                substitui(parents[i].p, pop[j].p, d.numcand);
                break;
            }
        }
    }
}


// CROSSOVER

// Crossover de Um Ponto
void crossover_one_point(pchrom parents, struct info d, pchrom offspring)
{
    int i, j, point;

    for (i=0; i<d.popsize; i+=2)
    {
       if (rand_01() < d.pr)
       {
          point = random_l_h(0, d.numcand-1);
          for (j=0; j<point; j++)
          {
             offspring[i].p[j] = parents[i].p[j];
             offspring[i+1].p[j] = parents[i+1].p[j];
          }
          for (j=point; j<d.numcand; j++)
          {
             offspring[i].p[j]= parents[i+1].p[j];
             offspring[i+1].p[j] = parents[i].p[j];
          }
       }
       else
       {
          // Não houve crossover: Copia direta dos pais para os filhos
          substitui(offspring[i].p, parents[i].p, d.numcand);
          substitui(offspring[i+1].p, parents[i+1].p, d.numcand);
       }

       // Reparar sempre
       //repara(offspring[i].p, d);
       //repara(offspring[i+1].p, d);
    }
}

// Crossover Uniforme
void crossover_uniforme(pchrom parents, struct info d, pchrom offspring)
{
    int i, j, mask;

    for (i=0; i<d.popsize; i+=2)
    {
       if (rand_01() < d.pr)
       {
          for (j=0; j<d.numcand; j++) {
             mask = flip();
             if (mask==1) // Troca
             {
                offspring[i].p[j] = parents[i+1].p[j];
                offspring[i+1].p[j] = parents[i].p[j];
             }
             else // Mantem
             {
                offspring[i].p[j]= parents[i].p[j];
                offspring[i+1].p[j] = parents[i].p[j];
             }
          }
       }
       else
       {
          // Não houve crossover: Copia
          substitui(offspring[i].p, parents[i].p, d.numcand);
          substitui(offspring[i+1].p, parents[i+1].p, d.numcand);
       }
       // Reparar sempre
       //repara(offspring[i].p, d);
       //repara(offspring[i+1].p, d);
    }
}

//MUTAÇÃO

// Mutação Bit-Flip
void mutation_bitflip(pchrom offspring, struct info d)
{
    int i, j;
    for (i=0; i<d.popsize; i++) {
       for (j=0; j<d.numcand; j++) {
          if (rand_01() < d.pm) {
             offspring[i].p[j] = !offspring[i].p[j];
          }
       }
       //repara(offspring[i].p, d);
    }
}

// Mutação Swap (Troca)
void mutation_swap(pchrom offspring, struct info d) {
    int i, p1, p2;

    for (i=0; i<d.popsize; i++){
       if (rand_01() < d.pm) {

          do {
             p1 = random_l_h(0, d.numcand-1);
          } while (offspring[i].p[p1] != 0);


          do {
             p2 = random_l_h(0, d.numcand-1);
          } while (offspring[i].p[p2] != 1);

          // Troca
          offspring[i].p[p1] = 1;
          offspring[i].p[p2] = 0;
       }
    }
}

//OPERADORES GERAIS

void genetic_operators(pchrom parents, struct info d, pchrom offspring) {
    // Recombinação (Escolher uma)
    crossover_one_point(parents, d, offspring);
    //crossover_uniforme(parents, d, offspring);

    // Mutação (Escolher uma)
    mutation_bitflip(offspring, d);
    //mutation_swap(offspring, d);
}