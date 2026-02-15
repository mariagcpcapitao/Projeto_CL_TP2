#define _CRT_SECURE_NO_WARNINGS 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "algoritmo.h"
#include "funcao.h"
#include "utils.h"

#define DEFAULT_RUNS 10

void pesquisalocal(int runs, struct info x, int **best_ptr, double *mbf, double *best_custo) {
    int k, itera;
    double custo;

    // Alocar solução temporária
    int *sol = malloc(sizeof(int) * x.numcand);
    int *melhor_sol_local = malloc(sizeof(int) * x.numcand);

    if(sol == NULL || melhor_sol_local == NULL) {
        printf("Erro na alocacao de memoria\n");
        exit(1);
    }

    printf("Indique o numero de iteracoes do trepa colinas: ");
    scanf("%d", &itera);

    // Inicializar melhor solução local
    gera_sol_inicial(melhor_sol_local, x);
    *best_custo = calcula_custo(melhor_sol_local, x);

    for(k = 0; k < runs; k++) {
        printf("\n--- Repeticao %d ---\n", k + 1);

        // Gerar nova solução inicial
        gera_sol_inicial(sol, x);

        printf("Solucao inicial: ");
        escreve_sol(sol, x);
        // Executar trepa-colinas
        custo = trepa_colinas(sol, x, itera);

        printf("Solucao final: ");
        escreve_sol(sol, x);

        *mbf += custo;

        // Atualizar melhor solução
        if(custo > *best_custo) {
            *best_custo = custo;
            substitui(melhor_sol_local, sol, x.numcand);
        }
    }

    // Copiar melhor solução para o ponteiro de retorno
    if(*best_ptr != NULL) {
        free(*best_ptr);
    }
    *best_ptr = malloc(sizeof(int) * x.numcand);
    if(*best_ptr == NULL) {
        printf("Erro na alocacao de memoria\n");
        exit(1);
    }
    substitui(*best_ptr, melhor_sol_local, x.numcand);

    // Liberar memória local
    free(sol);
    free(melhor_sol_local);
}


void algoritmoevolutivo(int runs, struct info x, int **best_ptr, double *mbf, double *best_custo)  {
    int r, inv, gen_actual, i, k;
    chrom best_ever;
    pchrom pop = NULL, parents = NULL;
    int selection_method; // 1: Tournament, 2: Roulette
    int invalid_strategy; // 1: Repair, 2: Penalty
    //printf("Metodo de Selecao (1: Torneio, 2: Roleta): ");
    //scanf("%d", &selection_method);
    selection_method=1;
    //printf("Estrategia para Invalidos (1: Reparacao, 2: Penalizacao): ");
    //scanf("%d", &invalid_strategy);
    invalid_strategy=1;

    chrom best_run_copy; // Nova estrutura para a cópia segura
    best_run_copy.p = malloc(sizeof(int) * x.numcand);
    if (!best_run_copy.p) exit(1);

    // Alocar memoria para o vetor da melhor solucao global
    best_ever.p = malloc(sizeof(int) * x.numcand);
    best_ever.fitness = 0.0;



    for (r=0; r<runs; r++)
    {
        printf("Repeticao %d\n", r+1);

        pop = init_pop(x);
        evaluate(pop, x, invalid_strategy);
        int best_idx = 0;
        for(i=1; i<x.popsize; i++) {
            if(pop[i].fitness > pop[best_idx].fitness) best_idx = i;
        }
        substitui(best_run_copy.p, pop[best_idx].p, x.numcand);
        best_run_copy.fitness = pop[best_idx].fitness;

        // Alocacao correcta dos pais
        parents = malloc(sizeof(chrom)*x.popsize);
        if (!parents) exit(1);
        for(i=0; i<x.popsize; i++) parents[i].p = malloc(sizeof(int)*x.numcand);

        gen_actual = 1;
        while (gen_actual <= x.numGenerations)
        {
            if (selection_method == 1) {
                tournament(pop, x, parents);
            } else {
                roulette(pop, x, parents);
            }
            genetic_operators(parents, x, pop);
            evaluate(pop, x, invalid_strategy);

            for(i=0; i<x.popsize; i++) {
                if(pop[i].fitness > best_run_copy.fitness) {
                    substitui(best_run_copy.p, pop[i].p, x.numcand);
                    best_run_copy.fitness = pop[i].fitness;
                }
            }

            gen_actual++;
        }

        // Estatisticas
        for (inv=0, i=0; i<x.popsize; i++)
            if (pop[i].valido == 0) inv++;

        printf("\nRepeticao %d:", r+1);
        // Usa a CÓPIA segura para o output da repetição
        escreve_sol(best_run_copy.p, x);
        printf("\nPercentagem Invalidos: %f\n", 100*(float)inv/x.popsize);

        // Usa o fitness da CÓPIA segura para o MBF
        *mbf += best_run_copy.fitness;

        // Atualizar Best Ever com copia (usando a cópia segura)
        if (r==0 || best_run_copy.fitness > best_ever.fitness) {
            best_ever.fitness = best_run_copy.fitness;
            best_ever.valido = best_run_copy.valido;
            substitui(best_ever.p, best_run_copy.p, x.numcand);

        }

        // Libertar memoria
        for(i=0; i<x.popsize; i++) free(parents[i].p);
        free(parents);
        free_pop(pop, x.popsize);
    }

    // Retornar resultado
    *best_custo = best_ever.fitness;

    // Preparar ponteiro de retorno para a main
    if(*best_ptr != NULL) free(*best_ptr);
    *best_ptr = malloc(sizeof(int) * x.numcand);

    // Usa a substitui para copiar o melhor vetor guardado
    substitui(*best_ptr, best_ever.p, x.numcand);

    free(best_ever.p);
    // NOVO: Libertar a memória da cópia temporária (best_run_copy)
    free(best_run_copy.p);
}
void algoritmohibrido(int runs, struct info x, int **best_ptr, double *mbf, double *best_custo) {
    int r, inv, gen_actual, i, k;
    int tipo_hibrido, itera_pl;
    chrom best_run, best_ever;
    pchrom pop = NULL, parents = NULL;
    int selection_method; // 1: Tournament, 2: Roulette
    int invalid_strategy; // 1: Repair, 2: Penalty



    // Menu de configuração do Híbrido
    printf("\n--- Configuracao Hibrido ---\n");
    printf("1. Refinamento Final (EA completo + PL no fim)\n");
    printf("2. Memetico (PL durante a evolucao no melhor individuo)\n");
    printf("Escolha o tipo (1 ou 2): ");
    scanf("%d", &tipo_hibrido);

    printf("Numero de iteracoes para o Trepa-Colinas: ");
    scanf("%d", &itera_pl);

    printf("Metodo de Selecao (1: Torneio, 2: Roleta): ");
    scanf("%d", &selection_method);
    printf("Estrategia para Invalidos (1: Reparacao, 2: Penalizacao): ");
    scanf("%d", &invalid_strategy);

    // Alocar memoria para o vetor da melhor solucao global
    best_ever.p = malloc(sizeof(int) * x.numcand);
    best_ever.fitness = 0.0;

    for (r=0; r<runs; r++)
    {
        printf("\nRepeticao %d\n", r+1);

        // Inicializar População
        pop = init_pop(x);
        evaluate(pop, x, invalid_strategy);

        best_run = pop[0];
        best_run = get_best(pop, x, best_run);

        parents = malloc(sizeof(chrom)*x.popsize);
        if (!parents) exit(1);
        for(i=0; i<x.popsize; i++) parents[i].p = malloc(sizeof(int)*x.numcand);


        gen_actual = 1;
        while (gen_actual <= x.numGenerations)
        {
            // Seleção, Cruzamento e Mutação
            tournament(pop, x, parents);
            genetic_operators(parents, x, pop);
            evaluate(pop, x, invalid_strategy); // Avalia e Repara

            // Atualiza melhor da geração
            best_run = get_best(pop, x, best_run);


            //2
            // A cada x(50) gerações aplica PL ao melhor indivíduo
            if (tipo_hibrido == 2 && gen_actual % 50 == 0) {
                // Encontrar o índice do melhor indivíduo na população atual
                int best_idx = 0;
                for(int j=1; j<x.popsize; j++) {
                    if(pop[j].fitness > pop[best_idx].fitness) best_idx = j;
                }


                double novo_custo = trepa_colinas(pop[best_idx].p, x, itera_pl);

                // Atualiza o fitness do indivíduo na população
                pop[best_idx].fitness = novo_custo;


                if(pop[best_idx].fitness > best_run.fitness) {
                    best_run = pop[best_idx];
                }
            }

            gen_actual++;
        }

        // 1
        if (tipo_hibrido == 1) {
            //printf(">> A aplicar Pesquisa Local na melhor solucao encontrada...\n");
            // Aplica Trepa-Colinas na melhor solução encontrada pelo EA

            //vetor temporário para a PL
            int *temp_sol = malloc(sizeof(int) * x.numcand);
            substitui(temp_sol, best_run.p, x.numcand);

            double custo_pos_pl = trepa_colinas(temp_sol, x, itera_pl);

            // Se melhorou atualizar o best_run
            if (custo_pos_pl > best_run.fitness) {
                best_run.fitness = custo_pos_pl;
            }

            if (custo_pos_pl > best_ever.fitness || r == 0) {
                 best_ever.fitness = custo_pos_pl;
                 best_ever.valido = 1;
                 substitui(best_ever.p, temp_sol, x.numcand);
            }
            free(temp_sol);

        } else {
            if (r==0 || best_run.fitness > best_ever.fitness) {
                best_ever.fitness = best_run.fitness;
                best_ever.valido = best_run.valido;
                substitui(best_ever.p, best_run.p, x.numcand);
            }
        }

        // Estatisticas
        for (inv=0, i=0; i<x.popsize; i++)
            if (pop[i].valido == 0) inv++;

        printf("\nRepeticao %d Terminada.", r+1);
        escreve_sol(best_run.p, x);
        printf("\nPercentagem Invalidos: %f\n", 100*(float)inv/x.popsize);

        *mbf += best_ever.fitness; // Usamos best_ever da run (que inclui PL)

        // Limpeza da run
        for(i=0; i<x.popsize; i++) free(parents[i].p);
        free(parents);
        free_pop(pop, x.popsize);
    }

    //resultados finais
    *best_custo = best_ever.fitness;

    // Preparar ponteiro de retorno para a main
    if(*best_ptr != NULL) free(*best_ptr);
    *best_ptr = malloc(sizeof(int) * x.numcand);
    substitui(*best_ptr, best_ever.p, x.numcand);

    free(best_ever.p);
}

int main(int argc, char *argv[]) {
    char nome_fich[100];
    struct info EA_param;
    double mbf = 0.0, best_custo = 0.0;
    int *best = NULL;
    int runs, tipoal = 0;

    printf("Trabalho Pratico N2\n");
    printf("Problema de Otimizacao\n");
    printf("No nosso trabalho foram implementados 3 algoritmos:\n");

    while(tipoal != 4) {
        printf("\n1 - Pesquisa Local (Trepa-Colinas)\n");
        printf("2 - Algoritmo evolutivo\n");
        printf("3 - Algoritmo hibrido\n");
        printf("4 - Sair\n");
        printf("Escolha (1-4): ");
        scanf("%d", &tipoal);

        if(tipoal == 4) break;

        // Obter nome do ficheiro
        if(argc == 3) {
            runs = atoi(argv[2]);
            strcpy(nome_fich, argv[1]);
        } else if(argc == 2) {
            runs = DEFAULT_RUNS;
            strcpy(nome_fich, argv[1]);
        } else {
            runs = DEFAULT_RUNS;
            //printf("Nome do Ficheiro: ");
            //scanf("%s", nome_fich);
            strcpy(nome_fich, "tourism_500.txt");
        }

        if(runs <= 0) return 0;

        // Inicializar e carregar dados
        init_rand();
        EA_param = init_data(nome_fich);

        printf("\nNumero de locais a selecionar: %d\n", EA_param.numlocais);
        printf("Numero total de candidatos: %d\n", EA_param.numcand);

        // Resetar variáveis para nova execução
        mbf = 0.0;
        best_custo = 0.0;

        switch(tipoal) {
            case 1:
                printf("Algoritmo escolhido: Trepa-Colinas\n");
                pesquisalocal(runs, EA_param, &best, &mbf, &best_custo);
                break;

            case 2:
                printf("Algoritmo evolutivo \n");
                algoritmoevolutivo(runs, EA_param, &best, &mbf, &best_custo);
                break;

            case 3:
                printf("Algoritmo Hibrido\n");
                algoritmohibrido(runs, EA_param, &best, &mbf, &best_custo);
                break;

            default:
                printf("Valor invalido\n");
                break;
        }
        printf("\nRESULTADOS FINAIS\n");
        printf("MBF (Media das melhores solucoes): %.6f\n", mbf / runs);
        printf("Melhor solucao: ");
        escreve_sol(best, EA_param);
        printf("\n");
    }

    // Liberar memória final
    free_info(EA_param);
    if(best != NULL) {
        free(best);
        best = NULL;
    }

    return 0;
}