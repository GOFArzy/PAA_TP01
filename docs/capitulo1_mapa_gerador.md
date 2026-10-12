# Capítulo 1 — Gerenciamento de Memória do Mapa e Algoritmo Gerador em C

## 1.1 Introdução e Responsabilidades do Módulo

O módulo do mapa é responsável pelo gerenciamento de dados fundamental do resolvedor de labirintos. Suas atribuições englobam:
1. Definição das estruturas de dados centrais utilizadas por todos os demais módulos do sistema (`Labirinto`, `TipoCelula`, `Posicao`, `Caminho`).
2. Leitura, validação sintática/semântica e carregamento do labirinto a partir de arquivos texto (`mapa.c`).
3. Gerenciamento estrito de memória dinâmica (`malloc`, `calloc`, `realloc` e `free`), garantindo a inexistência de vazamentos (*memory leaks*).
4. Implementação do programa gerador de labirintos solúveis (`gerador.c`), que utiliza uma caminhada aleatória com busca em profundidade (DFS) para garantir a existência de ao menos uma rota viável até o baú com todas as chaves requeridas.

---

## 1.2 Estruturas de Dados Projetadas

Para representar o labirinto de maneira compacta e eficiente em C99 puro, foram concebidas as seguintes estruturas no cabeçalho `mapa.h`:

### Enumeração das Células (`TipoCelula`)
```c
typedef enum {
    SALA_VAZIA  = 0,  /* '0' - sala livre para passagem */
    PAREDE      = 1,  /* '1' - obstaculo intransponivel */
    CHAVE       = 2,  /* 'C' - sala contendo chave */
    BAU         = 3,  /* 'X' - sala com o bau do tesouro */
    AVENTUREIRO = 4   /* 'A' - posicao inicial */
} TipoCelula;
```
O uso da enumeração garante legibilidade no código e controle de tipo nos acessos à matriz, mapeando diretamente os símbolos definidos na especificação do trabalho.

### Estrutura do Labirinto (`Labirinto`)
```c
typedef struct {
    int linhas;
    int colunas;
    int chaves_necessarias;
    TipoCelula **grade;
    int inicio_lin;
    int inicio_col;
    int bau_lin;
    int bau_col;
} Labirinto;
```
- `grade`: matriz bidimensional de ponteiros alocada dinamicamente, permitindo labirintos de quaisquer dimensões limitadas apenas pela memória física.
- `inicio_lin`, `inicio_col`: coordenadas pré-indexadas da posição de largada `A`.
- `bau_lin`, `bau_col`: coordenadas pré-indexadas do baú `X`.
- `chaves_necessarias`: meta de chaves a serem coletadas antes de abrir o baú.

### Estrutura do Caminho Dinâmico (`Caminho`)
```c
typedef struct {
    int linha;
    int coluna;
} Posicao;

typedef struct {
    Posicao *passos;
    int tamanho;
    int capacidade;
} Caminho;
```
Implementada como um vetor dinâmico redimensionável (*dynamic array*). Inicia com capacidade para 64 passos e duplica de tamanho (`realloc`) sempre que atinge o limite, garantindo custo amortizado $O(1)$ por inserção durante o backtracking.

---

## 1.3 Gerenciamento de Memória

### Alocação da Grade
A matriz do labirinto é alocada em duas etapas:
1. Vetor de ponteiros para as linhas:
   ```c
   lab->grade = (TipoCelula **)malloc(lab->linhas * sizeof(TipoCelula *));
   ```
2. Vetor de células para cada linha:
   ```c
   lab->grade[i] = (TipoCelula *)malloc(lab->colunas * sizeof(TipoCelula));
   ```
Caso qualquer alocação intermediária falhe, todas as linhas previamente alocadas são liberadas em ordem reversa antes de desalocar a struct principal e retornar `NULL`, evitando estados inconsistentes.

### Liberação de Recursos
A função `liberar_labirinto(Labirinto *lab)` percorre linha a linha liberando a memória alocada, seguida da liberação do vetor de ponteiros e, por fim, da estrutura raiz:
```c
void liberar_labirinto(Labirinto *lab) {
    if (!lab) return;
    if (lab->grade) {
        for (int i = 0; i < lab->linhas; i++) {
            free(lab->grade[i]);
        }
        free(lab->grade);
    }
    free(lab);
}
```

O mesmo padrão rigoroso é aplicado à matriz auxiliar de visitados (`alocar_matriz_int` e `liberar_matriz_int`), garantindo que nenhum bloco de memória permaneça órfão.

---

## 1.4 Algoritmo do Gerador de Mapas (`gerador.c`)

O gerador de mapas implementado em 100% C puro assegura que qualquer mapa gerado aleatoriamente seja **estritamente solúvel**. Para alcançar essa propriedade, adota-se a seguinte estratégia:

1. **Caminho Base Garantido por DFS Randomizada**:
   - A grade é inicializada como um campo inteiramente aberto.
   - Posiciona-se o aventureiro em `(0, 0)` e o baú em `(linhas - 1, colunas - 1)`.
   - Uma busca em profundidade com sorteio aleatório de direções (Fisher-Yates shuffle) encontra um caminho simples e sem repetições conectando `A` a `X`.

2. **Distribuição das Chaves no Caminho**:
   - As células pertencentes ao caminho encontrado são identificadas.
   - Um subconjunto aleatório dessas posições intermediárias (excluindo a largada e o baú) é convertido em chaves (`C`). Como todas as chaves estão situadas sobre o caminho primário, é garantido que o aventureiro possa alcançá-las antes de finalizar a rota.

3. **Geração de Barreiras e Salas**:
   - As células do caminho protegido são preservadas como navegáveis.
   - As demais células da grade recebem paredes (`1`) conforme uma probabilidade parametrizável (padrão: 35%), ou permanecem como salas livres (`0`).

### Interface de Linha de Comando do Gerador
```bash
./bin/gerador <linhas> <colunas> <num_chaves> <saida.txt> [seed] [prob_paredes]
```
Parâmetros opcionais permitem fixar a semente pseudoaleatória para reprodutibilidade científica e controlar a densidade de paredes.

---

## 1.5 Análise de Complexidade

| Operação / Módulo | Complexidade de Tempo | Complexidade de Espaço |
| :--- | :---: | :---: |
| Carregamento do Mapa (`carregar_labirinto`) | $O(L \times C)$ | $O(L \times C)$ |
| Liberação do Mapa (`liberar_labirinto`) | $O(L)$ | $O(1)$ |
| Inserção no Caminho (`caminho_adicionar`) | $O(1)$ amortizado | $O(N)$ passos |
| Geração de Mapa (`gerador.c`) | $O(L \times C)$ | $O(L \times C)$ |

- **Tempo**: A leitura e a geração percorrem a grade de dimensões $L \times C$ um número constante de vezes, tornando o algoritmo linear em relação ao número total de células do labirinto.
- **Espaço**: A representação matricial armazena exatamente $L \times C$ células, demandando memória proporcional ao tamanho do labirinto.
