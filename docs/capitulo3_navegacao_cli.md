# Capítulo 3 — Regras de Transição, Movimentação Toroidal e Tratamento de CLI

> **Disciplina:** Projeto e Análise de Algoritmos — UFV Florestal  
> **Professor:** Daniel Mendes Barbosa  
> **Trabalho Prático 1**  
> **Integrante responsável por este capítulo:** [SEU NOME] — Matrícula: [SEU NÚMERO]

---

## Sumário do Capítulo

1. Introdução
2. Módulo de Navegação (navegacao.c)
   - 2.1 Estruturas de dados utilizadas
   - 2.2 Regras de transição em modo normal
   - 2.3 Modo Globo — movimentação toroidal (Extra 3)
3. Módulo de CLI (cli.c)
   - 3.1 Estrutura Opcoes
   - 3.2 Função cli_parse()
   - 3.3 Função cli_usage()
4. Decisões de implementação
5. O que funciona e o que não funciona
6. Conclusão
7. Referências

---

## 1. Introdução

Este capítulo descreve a implementação dos módulos de navegação (navegacao.c / navegacao.h)
e de interface de linha de comando (cli.c / cli.h), que compõem a parte 3 do trabalho.

O módulo de navegação é responsável por definir quais movimentos são permitidos a cada passo
do algoritmo de backtracking: valida limites do labirinto, verifica paredes e células já
visitadas. Ele também implementa o Modo Globo (Extra 3), em que as bordas do labirinto se
conectam de forma toroidal.

O módulo de CLI é responsável por interpretar os argumentos passados ao programa na linha de
comando, permitindo ativar modos especiais como --globo, --todos, --analise e --visual sem
modificar o código-fonte.

Ambos os módulos foram escritos em C99 puro, sem dependências externas, utilizando apenas
<stdio.h>, <string.h> e os headers do próprio projeto.

---

## 2. Módulo de Navegação (navegacao.c)

### 2.1 Estruturas de dados utilizadas

O módulo não define estruturas próprias; opera sobre os tipos definidos em mapa.h:

- Labirinto — contém a grade (grade[lin][col]), as dimensões (linhas, colunas) e as posições
  especiais (início, baú).
- int **visitado — matriz de inteiros alocada dinamicamente pelo módulo de backtracking, onde
  visitado[l][c] = 1 indica que a célula já foi explorada.
- TipoCelula — enumeração que classifica cada célula: SALA_VAZIA, PAREDE, CHAVE, BAU,
  AVENTUREIRO.

Para representar as quatro direções de movimento, o módulo usa dois vetores constantes:

    const int DIRECOES_LIN[NUM_DIRECOES] = {-1, 1, 0, 0};
    const int DIRECOES_COL[NUM_DIRECOES] = { 0, 0,-1, 1};

O índice d (de 0 a 3) seleciona, respectivamente: cima, baixo, esquerda, direita. Essa
representação evita quatro blocos if/else separados e facilita iterar as direções em um laço
for (d = 0; d < 4; d++) no backtracking.

### 2.2 Regras de transição em modo normal

A função central do módulo é movimento_valido(), que aplica as três restrições do problema:

    int movimento_valido(const Labirinto *lab, int **visitado, int lin, int col) {
      /* Regra 1 — dentro dos limites da grade */
      if (lin < 0 || lin >= lab->linhas)  return 0;
      if (col < 0 || col >= lab->colunas) return 0;

      /* Regra 2 — não é parede */
      if (lab->grade[lin][col] == PAREDE) return 0;

      /* Regra 3 — não foi visitada ainda */
      if (visitado[lin][col]) return 0;

      return 1;
    }

A ordem das verificações é importante: a checagem de limites DEVE ocorrer antes do acesso à
grade, pois acessar lab->grade[lin][col] com coordenadas fora dos limites causaria
comportamento indefinido (acesso inválido de memória).

A função proxima_posicao() combina o cálculo do deslocamento com a validação:

    int proxima_posicao(const Labirinto *lab, int **visitado, int lin, int col,
                        int d, int *prox_lin, int *prox_col) {
      *prox_lin = lin + DIRECOES_LIN[d];
      *prox_col = col + DIRECOES_COL[d];
      return movimento_valido(lab, visitado, *prox_lin, *prox_col);
    }

O backtracking chama proxima_posicao() para cada direção d e só avança se o retorno for 1.

### 2.3 Modo Globo — movimentação toroidal (Extra 3)

No Modo Globo, o labirinto é tratado como um toroide: sair pela borda direita equivale a
entrar pela borda esquerda, e sair pela borda inferior equivale a entrar pela borda superior
— semelhante ao comportamento da cobrinha no jogo clássico.

Isso é implementado com aritmética modular:

    nova_lin = (lin + DIRECOES_LIN[d] + lab->linhas)  % lab->linhas
    nova_col = (col + DIRECOES_COL[d] + lab->colunas) % lab->colunas

O termo "+ lab->linhas" antes do módulo é essencial para garantir resultado positivo quando
o delta é negativo. Por exemplo, se o aventureiro está na linha 0 e se move para cima (delta
= -1):

    nova_lin = (0 + (-1) + L) % L = (L - 1) % L = L - 1

Sem o somador, (-1) % L em C resulta em -1 — um índice inválido — pois o operador % em C
preserva o sinal do dividendo (diferente de Python, onde % sempre retorna não-negativo).

    int proxima_posicao_globo(const Labirinto *lab, int **visitado, int lin,
                              int col, int d, int *prox_lin, int *prox_col) {
      *prox_lin = (lin + DIRECOES_LIN[d] + lab->linhas)  % lab->linhas;
      *prox_col = (col + DIRECOES_COL[d] + lab->colunas) % lab->colunas;
      return movimento_valido_globo(lab, visitado, *prox_lin, *prox_col);
    }

A variante movimento_valido_globo() é simplificada: após o wrap-around, as coordenadas já
estão necessariamente dentro dos limites, então a checagem de limites é suprimida.

A escolha entre modo normal e modo globo é feita pelo backtracking com base no campo
opts->modo_globo da struct Opcoes.

---

## 3. Módulo de CLI (cli.c / cli.h)

### 3.1 Estrutura Opcoes

Toda a configuração do programa é centralizada na struct Opcoes, definida em cli.h:

    typedef struct {
        const char *arquivo_entrada; /* caminho do arquivo .txt (obrigatório) */
        int modo_globo;              /* --globo: bordas toroidais */
        int modo_todos;              /* --todos: encontrar todos os caminhos */
        int modo_analise;            /* --analise: exibir métricas de recursão */
        int modo_visual;             /* --visual: visualizador ANSI no terminal */
    } Opcoes;

O uso de uma struct centraliza todas as opções em um único ponto, facilitando a passagem de
parâmetros entre módulos (basta passar const Opcoes *opts) e tornando simples adicionar
novas flags no futuro.

### 3.2 Função cli_parse()

cli_parse() percorre argv do índice 1 ao fim e classifica cada argumento:

- Se começa com --, é tratado como flag e comparado com as opções conhecidas via strcmp().
- Se não começa com --, é tratado como o caminho do arquivo de entrada.
- Flags desconhecidas emitem um aviso em stderr mas NÃO abortam o programa.

    for (int i = 1; i < argc; i++) {
        if      (strcmp(argv[i], "--globo")   == 0) opts->modo_globo   = 1;
        else if (strcmp(argv[i], "--todos")   == 0) opts->modo_todos   = 1;
        else if (strcmp(argv[i], "--analise") == 0) opts->modo_analise = 1;
        else if (strcmp(argv[i], "--visual")  == 0) opts->modo_visual  = 1;
        else if (argv[i][0] == '-' && argv[i][1] == '-')
            fprintf(stderr, "Aviso: opcao desconhecida '%s' ignorada.\n", argv[i]);
        else if (opts->arquivo_entrada == NULL)
            opts->arquivo_entrada = argv[i];
    }

Após o laço, se arquivo_entrada permanecer NULL, a função chama cli_usage() e retorna 0
(falha). As flags podem ser passadas em qualquer ordem:

    ./labirinto entrada.txt --analise --globo
    ./labirinto --visual entrada.txt --todos

### 3.3 Função cli_usage()

Imprime em stderr a sintaxe esperada e exemplos de uso, sendo chamada automaticamente quando
o usuário não informa o arquivo de entrada. Exemplo de saída:

    Uso: ./labirinto <arquivo_labirinto.txt> [opcoes]

    Opcoes:
      --globo    Ativa modo globo (labirinto toroidal, bordas se conectam)
      --todos    Encontra e exibe todos os caminhos possiveis ate o bau
      --analise  Exibe metricas de recursao (chamadas totais e nivel maximo)
      --visual   Visualizador ANSI: imprime o labirinto colorido no terminal

    Exemplo:
      ./labirinto entrada.txt --analise
      ./labirinto entrada.txt --globo --visual

---

## 4. Decisões de implementação

| Decisão | Justificativa |
|---------|---------------|
| Vetores DIRECOES_LIN / DIRECOES_COL em vez de switch/if por direção | Elimina código repetitivo e permite iterar todas as direções com um único for no backtracking |
| "+ lab->linhas" antes do módulo no modo globo | Garante resultado positivo em C, onde % preserva o sinal do dividendo |
| Verificação de limites ANTES do acesso à grade | Evita acesso inválido de memória (undefined behavior em C) |
| Flags desconhecidas geram aviso mas não abortam | Robustez: o usuário pode digitar errado uma flag e o programa ainda executa normalmente |
| "const Labirinto *lab" nos parâmetros | Sinaliza que navegacao.c não modifica o labirinto — apenas lê |
| Struct Opcoes centralizando todas as flags | Facilita passar configuração entre módulos com um único ponteiro |

---

## 5. O que funciona e o que não funciona

FUNCIONA:
- Validação completa de movimentos (limites, paredes, visitados) em modo normal.
- Movimentação toroidal com aritmética modular correta para todas as bordas (cima, baixo,
  esquerda, direita).
- Parse de todas as quatro flags (--globo, --todos, --analise, --visual).
- Tratamento de flags em qualquer ordem na linha de comando.
- Exibição de mensagem de uso clara quando o arquivo não é informado.
- Aviso não-fatal para flags desconhecidas.

LIMITAÇÕES CONHECIDAS:
- O módulo de navegação não decide o que fazer ao chegar em uma chave ou no baú; essa
  responsabilidade é do módulo de backtracking, que recebe a célula destino e toma a decisão.
- cli_parse() armazena um ponteiro para o argv original (const char *arquivo_entrada), válido
  apenas durante a execução do main. Não deve ser copiado para uso após o término do programa.

---

## 6. Conclusão

A implementação dos módulos de navegação e CLI demonstrou como separar responsabilidades em
C puro de forma limpa: o módulo de navegação encapsula as regras físicas do labirinto (o que
é um movimento válido), enquanto o módulo de CLI encapsula a interface com o usuário (como o
programa é configurado). Essa separação facilita a integração com os demais módulos do grupo,
especialmente o backtracking, que apenas chama proxima_posicao() ou proxima_posicao_globo()
sem precisar conhecer os detalhes de validação.

O Extra 3 (Modo Globo) foi um ponto de aprendizado relevante: a necessidade de somar o
tamanho da dimensão antes de aplicar o módulo (% L) para evitar índices negativos em C é uma
característica importante do operador % que difere de linguagens como Python.

---

## 7. Referências

- CORMEN, T. H. et al. Introduction to Algorithms. 3a ed. MIT Press, 2009. — Base teórica
  para análise de algoritmos de busca e backtracking.
- ISO/IEC 9899:1999 (C99). Comportamento do operador % com operandos negativos (paragrafo
  6.5.5).
- Especificação do Trabalho Prático 1 — PAA 2026/2, Prof. Daniel Mendes Barbosa.
- Diretrizes básicas para as documentações da disciplina — PAA, UFV Florestal.
