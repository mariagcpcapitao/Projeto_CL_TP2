#include "funcao.h"
double calcula_custo(int sol[], struct info x) {
    double soma = 0.0;
    int i, j;
    int count = 0;

    // Contar quantos locais foram selecionados
    for(i = 0; i < x.numcand; i++) {
        if(sol[i] == 1) count++;
    }

    // Verificar se tem exatamente m locais (só para debug)
    if(count != x.numlocais) {
        printf("AVISO: Solucao tem %d locais, devia ter %d\n",
               count, x.numlocais);
        return 0.0; //penalização cega
    }
    // Calcular soma das distâncias
    for(i = 0; i < x.numcand; i++) {
        if(sol[i] == 1) {
            for(j = i + 1; j < x.numcand; j++) {
                if(sol[j] == 1) {
                    soma += x.distancias[i][j];
                }
            }
        }
    }


    if(x.numlocais > 1) {
        return  soma / x.numlocais ;
    }
    return 0.0;
}

// Penalização proporcional ao erro
double penalizacao(int *sol, struct info x) {
    int i, count = 0;

    // Contar pontos selecionados
    for(i = 0; i < x.numcand; i++) {
        if(sol[i] == 1) count++;
    }

    // Solução VÁLIDA: tem exatamente m pontos
    if(count == x.numlocais) {
        return calcula_custo(sol, x);
    }

    // Solução INVÁLIDA: penalização = -1000 * erro
    int erro = abs(count - x.numlocais);
    return -1000.0 * erro;
}

// Funcao de Reparacao: Garante m locais selecionados
void repara(int *sol, struct info d) {
    int count = 0, i, pos;
    for(i=0; i<d.numcand; i++) if(sol[i]) count++;

    // Se tem a mais, remove aleatoriamente
    while(count > d.numlocais) {
        pos = random_l_h(0, d.numcand-1);
        if(sol[pos] == 1) { sol[pos] = 0; count--; }
    }
    // Se tem a menos, adiciona aleatoriamente
    while(count < d.numlocais) {
        pos = random_l_h(0, d.numcand-1);
        if(sol[pos] == 0) { sol[pos] = 1; count++; }
    }
}
void evaluate(pchrom pop, struct info d, int invalid_strategy) { // ALTERADO
    for (int i = 0; i < d.popsize; i++) {

        // 1: Reparação (Estratégia atual)
        if (invalid_strategy == 1) {
            repara(pop[i].p, d);
            pop[i].valido = 1;
            pop[i].fitness = calcula_custo(pop[i].p, d);
        }

        // 2: Penalização (Alternativa)
        else {
            pop[i].fitness = penalizacao(pop[i].p, d);
            int count = 0;
            for(int j = 0; j < d.numcand; j++) {
                if(pop[i].p[j] == 1) count++;
            }
            pop[i].valido = (count == d.numlocais) ? 1 : 0;
        }

    }
}
