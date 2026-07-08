# Trabalho Prático 2 — Problema de Otimização

Implementação em C de três metaheurísticas para resolver um **problema de seleção/dispersão de locais**: dado um conjunto de candidatos (locais turísticos) e as distâncias entre eles, o objetivo é escolher um subconjunto de tamanho fixo que **maximize a distância média entre os locais escolhidos**.

## O problema

- Input: um número de candidatos (`numcand`), um número de locais a selecionar (`numlocais`), e a distância entre cada par de candidatos.
- Output: um subconjunto de `numlocais` candidatos (representado como vetor binário) que maximiza a distância média entre pares selecionados.
- Ficheiro de dados por omissão: `tourism_500.txt`, com o formato:
  <numcand> <numlocais>
e1 e2 <distancia>
e1 e3 <distancia>
...
  (grafo não-direcionado: a distância entre e1-e2 é igual a e2-e1)

## Algoritmos implementados

O programa apresenta um menu interativo com 3 opções:

1. **Pesquisa Local (Trepa-Colinas / Hill Climbing)** — `pesquisalocal()` em `main.c`, com a lógica de vizinhança em `algoritmo.c` (`gera_vizinho_simples`, `trepa_colinas`). A cada iteração troca um local selecionado por um não selecionado e aceita a troca se não piorar a solução.

2. **Algoritmo Evolutivo (Genético)** — `algoritmoevolutivo()`. Cada indivíduo é um vetor binário (1 = local selecionado). Inclui:
   - Seleção por **torneio** (`tournament`) ou **roleta** (`roulette`)
   - **Crossover** de um ponto ou uniforme (`crossover_one_point`, `crossover_uniforme`)
   - **Mutação** bit-flip ou swap (`mutation_bitflip`, `mutation_swap`)
   - Tratamento de soluções inválidas (que não têm exatamente `numlocais` selecionados) por **reparação** ou **penalização** (`repara`, `penalizacao`)

3. **Algoritmo Híbrido (Memético)** — `algoritmohibrido()`. Combina o algoritmo evolutivo com pesquisa local, com duas variantes configuráveis:
   - **Refinamento final**: corre o EA completo e aplica Trepa-Colinas só no fim, à melhor solução
   - **Memético**: aplica Trepa-Colinas periodicamente (a cada 50 gerações) ao melhor indivíduo da população

Para cada algoritmo, o programa corre várias repetições (`runs`, por omissão 10) e reporta a melhor solução encontrada e o **MBF** (Mean Best Fitness — média das melhores soluções de cada repetição).

## Estrutura dos ficheiros

| Ficheiro | Conteúdo |
|---|---|
| `main.c` | Menu principal, leitura de argumentos, ciclo de execução dos 3 algoritmos |
| `algoritmo.c` / `.h` | Lógica dos algoritmos: vizinhança (pesquisa local), seleção, crossover, mutação (evolutivo) |
| `funcao.c` / `.h` | Função objetivo (`calcula_custo`), penalização e reparação de soluções inválidas |
| `utils.c` / `.h` | Leitura de dados, geração de números aleatórios, alocação/libertação de memória, funções auxiliares |
| `CMakeLists.txt` | Configuração do build (C23) |

## Como compilar e correr

```bash
mkdir build && cd build
cmake ..
make
./Projeto_CL_TP2 [ficheiro_de_dados] [numero_de_runs]
```

- Sem argumentos: usa `tourism_500.txt` e 10 runs por omissão.
- Com 1 argumento: nome do ficheiro de dados, 10 runs.
- Com 2 argumentos: nome do ficheiro de dados e número de runs.

No menu interativo, escolhe entre pesquisa local, evolutivo, híbrido ou sair, e segue as instruções apresentadas (número de iterações, método de seleção, estratégia para inválidos, etc.).

## Autor

Maria Capitão
