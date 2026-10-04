# PAA - Trabalho Prático 1 (Labirinto e Backtracking)

Este repositório contém a estrutura base em **100% C nativo (C99/C11)** para a resolução do Trabalho Prático 1. O código foi modularizado para que o grupo possa trabalhar em paralelo e sem conflitos, com as responsabilidades bem divididas.

## Estrutura do Repositório

Como é um projeto em C, **não se cria uma pasta para cada integrante**. Todos os arquivos fonte ficam juntos na pasta `src/` e os cabeçalhos (headers) na pasta `include/`. Cada membro do grupo é "dono" de arquivos específicos, conforme a divisão estipulada abaixo:

```text
PAA_TP01/
├── Makefile              # (Pessoa 5) Arquivo de compilação automática
├── README.md             # Instruções gerais do projeto
├── entradas/             # Arquivos de texto com os labirintos para teste
│   └── exemplo.txt       # Exemplo disponibilizado no PDF
├── docs/                 # Documentação (Relatório PDF) dividida em capítulos
│   ├── capitulo1_mapa_gerador.md
│   ├── capitulo2_backtracking.md
│   └── ...
├── include/              # Arquivos de cabeçalho (.h) - Contêm as assinaturas das funções
│   ├── mapa.h            # (Pessoa 1)
│   ├── backtracking.h    # (Pessoa 2)
│   ├── navegacao.h       # (Pessoa 3)
│   ├── cli.h             # (Pessoa 3)
│   ├── analise.h         # (Pessoa 4)
│   ├── visualizador.h    # (Pessoa 4)
│   └── saida.h           # (Pessoa 5)
└── src/                  # Código-fonte (.c) - Onde a lógica real acontece
    ├── main.c            # (Pessoa 5) Função main() principal do labirinto
    ├── mapa.c            # (Pessoa 1) Leitura e gerenciamento de memória do labirinto
    ├── gerador.c         # (Pessoa 1) Extra 1: Programa isolado que gera labirintos aleatórios
    ├── backtracking.c    # (Pessoa 2) Lógica recursiva principal (DFS)
    ├── navegacao.c       # (Pessoa 3) Regras de movimento (cima, baixo...) e modo Globo
    ├── cli.c             # (Pessoa 3) Leitura de argumentos (ex: --globo, --visual)
    ├── analise.c         # (Pessoa 4) Métricas e contagem de chamadas/nível de recursão
    ├── visualizador.c    # (Pessoa 4) Extra 4: Interface ANSI que imprime o mapa colorido
    ├── saida.c           # (Pessoa 5) Formatação e exibição do caminho encontrado
    └── benchmark.c       # (Pessoa 5) Extra 5: Script C para medir tempos de execução
```

## Divisão de Tarefas

### Pessoa 1: Módulo do Mapa e Gerador (Você)
**Seus arquivos:** `src/mapa.c`, `include/mapa.h`, `src/gerador.c`, `docs/capitulo1_mapa_gerador.md`
*   **O que fazer:** Criar as structs para o labirinto. Ler o arquivo `.txt`, alocar dinamicamente com `malloc` a matriz do mapa e limpá-la com `free`.
*   **Extra 1:** Implementar o arquivo `gerador.c` que usará `rand()` (da `<stdlib.h>`) para criar arquivos `.txt` de labirintos aleatórios válidos.

### Pessoa 2: Módulo de Backtracking
**Arquivos:** `src/backtracking.c`, `include/backtracking.h`, `docs/capitulo2_backtracking.md`
*   **O que fazer:** Implementar a função recursiva principal de Backtracking para caminhar pelo labirinto, coletar as chaves e achar o baú, marcando posições visitadas para não entrar em loop.
*   **Extra 2:** Modificar o backtracking para continuar rodando e acumular todas as rotas possíveis, se for pedido via CLI.

### Pessoa 3: Módulo de Navegação e Argumentos
**Arquivos:** `src/navegacao.c`, `include/navegacao.h`, `src/cli.c`, `include/cli.h`, `docs/capitulo3_navegacao_cli.md`
*   **O que fazer:** Fazer a lógica para checar se a próxima casa é válida (não é parede) e definir os deslocamentos (cima, baixo, esquerda, direita). Ler se o programa foi chamado com alguma flag especial (`cli.c`).
*   **Extra 3 (Modo Globo):** Usar aritmética modular para conectar as bordas do mapa (ex: sair pela direita e aparecer na esquerda).

### Pessoa 4: Módulo de Análise e Visualizador ANSI
**Arquivos:** `src/analise.c`, `include/analise.h`, `src/visualizador.c`, `include/visualizador.h`, `docs/capitulo4_analise_visualizador.md`
*   **O que fazer:** Criar variáveis/estruturas para contar nível máximo de recursão e quantidade de chamadas recursivas.
*   **Extra 4 (Terminal Visual):** Usar códigos ANSI para limpar a tela (`\033[H\033[J`) e pintar as células para mostrar a execução do aventureiro "animada" no terminal. Usar `usleep()` da `<unistd.h>`.

### Pessoa 5: Saída, Benchmark, Main e Makefile
**Arquivos:** `src/main.c`, `src/saida.c`, `include/saida.h`, `src/benchmark.c`, `Makefile`, `docs/capitulo5_saida_benchmark.md`
*   **O que fazer:** Juntar tudo no `main.c`. Pegar a lista de passos da recursão e imprimir exatamente no formato pedido: `[l,c],[l,c]...`. Fazer o Makefile para compilar tudo fácil (já deixei pronto!).
*   **Extra 5:** Fazer o executável separado (`benchmark.c`) que mede em milissegundos quanto tempo cada labirinto demora, usando `<time.h>`.

## Como compilar e testar

Para compilar o código de todos vocês:
1. Abra o terminal.
2. Digite `make`. (Isso criará a pasta `bin/` com os executáveis `labirinto`, `gerador` e `benchmark`).
3. Para rodar: `./bin/labirinto entradas/exemplo.txt`
4. Para limpar os binários: `make clean`
