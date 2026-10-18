# Capítulo 5 — Módulo de Saída, Orquestração e Benchmark em C

## 5.1 Introdução e Responsabilidades do Módulo

O módulo de saída e testes empíricos é responsável pela integração dos componentes do resolvedor de labirintos e pela mensuração de sua eficiência computacional. Suas atribuições englobam:
1. Padronização e formatação da sequência de passos no formato exigido pela especificação (`saida.c` e `saida.h`).
2. Orquestração e unificação do pipeline de execução no ponto de entrada principal (`main.c`).
3. Implementação do programa de avaliação empírica de desempenho (`benchmark.c`), com cálculo de média, desvio padrão e tempos extremos em milissegundos.
4. Automação do processo de compilação multiplataforma via diretivas de construção (`Makefile`).

---

## 5.2 Módulo de Formatação e Saída (`saida.c`)

### Especificação do Padrão de Rota
A especificação do trabalho requer que o trajeto percorrido pelo aventureiro até o baú seja exibido como uma lista de coordenadas no formato:

$$\text{Caminho} = [l_0, c_0],[l_1, c_1],\dots,[l_{k-1}, c_{k-1}]$$

Onde cada par $[l_i, c_i]$ representa a linha e a coluna da célula visitada no instante $i$.

### Funções do Cabeçalho `saida.h`
```c
/* Impressao da sequencia de coordenadas no formato [l,c],[l,c],... */
void imprimir_caminho(const Caminho *caminho);

/* Mensagem padrao quando nao ha solucao viavel */
void imprimir_sem_solucao(void);

/* Gravacao do caminho formatado em arquivo texto */
int salvar_caminho_arquivo(const char *nome_arquivo, const Caminho *caminho);

/* Exibicao de resumo com metricas da execucao */
void imprimir_resumo(int encontrou_solucao, const Caminho *caminho, double tempo_ms, long long chamadas_rec, int nivel_max_rec);
```

- `imprimir_caminho`: percorre o vetor dinâmico `Caminho` imprimindo cada tupla com colchetes e separando por vírgula, assegurando a terminação com `\n` sem vírgula residual.
- `imprimir_sem_solucao`: exibe notificação padronizada caso a busca por backtracking retorne que não há trajeto viável para o baú.
- `salvar_caminho_arquivo`: grava o trajeto em disco, facilitando rotinas de testes automatizados e conferência de gabaritos.

---

## 5.3 Orquestração e Integração no `main.c`

O programa principal centraliza os módulos do trabalho em um fluxo sequencial:

1. **Leitura de Argumentos**: Valida os parâmetros de entrada e opções adicionais (`cli.c`).
2. **Carregamento do Mapa**: Invoca `carregar_labirinto` para alocar e ler o arquivo (`mapa.c`).
3. **Métricas e Busca**: Inicializa as variáveis de análise (`analise.c`) e executa o algoritmo recursivo de exploração (`backtracking.c`).
4. **Exibição e Limpeza**: Apresenta a rota formatada via `imprimir_caminho` (`saida.c`) e desaloque toda a memória alocada (`liberar_labirinto` e `caminho_liberar`).

---

## 5.4 Algoritmo do Programa de Benchmark (`benchmark.c`)

O programa `benchmark.c` afere empiricamente a eficiência temporal do algoritmo resolvendo labirintos sob repetidas execuções para amortizar variações causadas pelo escalonador do sistema operacional.

1. **Amostragem Temporal**:
   - Para cada labirinto, a busca é executada $N$ vezes consecutivas (padrão: 10 repetições).
   - O tempo de cada iteração é medido em milissegundos através da função `clock()` da biblioteca `<time.h>`.

2. **Cálculo de Parâmetros Estatísticos**:
   - **Média Amostral ($\bar{x}$)**:
     $$\bar{x} = \frac{1}{N} \sum_{i=1}^{N} t_i$$
   - **Desvio Padrão Amostral ($s$)**:
     $$s = \sqrt{\frac{1}{N-1} \sum_{i=1}^{N} (t_i - \bar{x})^2}$$
   - Identificação dos tempos mínimo e máximo para detecção de variações atípicas (*outliers*).

### Interface de Linha de Comando do Benchmark
```bash
./bin/benchmark <labirinto1.txt> [labirinto2.txt ...] [repeticoes]
```
Permite testar um único mapa ou múltiplos arquivos em lote, parametrizando o volume de repetições desejado.

---

## 5.5 Análise de Complexidade

| Operação / Módulo | Complexidade de Tempo | Complexidade de Espaço |
| :--- | :---: | :---: |
| Impressão do Caminho (`imprimir_caminho`) | $O(K)$ | $O(1)$ |
| Gravação do Caminho (`salvar_caminho_arquivo`) | $O(K)$ | $O(1)$ |
| Execução do Benchmark (`executar_benchmark`) | $O(N \times T_{\text{busca}})$ | $O(N)$ |
| Orquestração Principal (`main.c`) | $O(T_{\text{busca}} + L \times C)$ | $O(L \times C)$ |

- **Tempo**: A exibição e a persistência do caminho dependem linearmente do número $K$ de passos que compõem a rota ($K \le L \times C$). O tempo total do benchmark é proporcional ao número $N$ de repetições multiplicado pelo tempo de resolução da busca.
- **Espaço**: As rotinas de saída operam sobre referências já alocadas, consumindo espaço auxiliar $O(1)$. O benchmark aloca um vetor estático de $N$ posições em ponto flutuante para cômputo do desvio padrão.
