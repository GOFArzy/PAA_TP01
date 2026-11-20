# PAA - Trabalho Prático 2: Escape da Caverna com Programação Dinâmica

Este repositório contém a implementação completa em **C (C99)** para o Trabalho Prático 2 de **Projeto e Análise de Algoritmos (PAA)** da Universidade Federal de Viçosa (UFV) — Campus Florestal.

O projeto resolve o problema de encontrar a rota que maximiza a quantidade final de Pontos de Vida (HP) de um estudante ao escapar de uma caverna bidimensional representada por uma matriz $L \times C$, movendo-se estritamente para **cima** e para a **esquerda**, sem que seus pontos de vida cheguem a zero ou fiquem negativos em nenhuma célula do trajeto.

---

## Integrantes do Grupo

* **Gabriel Marcus de Oliveira Felix** — Matrícula: 5792
* **Otávio Soares Pedroso** — Matrícula: 6593
* **Pedro Mendes Gonzaga** — Matrícula: 6563
* **Rafaella Ferreira Pinheiro Silva** — Matrícula: 5363
* **Tomás Viana Resende** — Matrícula: 6556

---

## Divisão de Módulos e Responsabilidades

1. **Rafa (Leitura e Representação do Mapa):** `include/mapa.h`, `src/mapa.c`, `testes/teste_mapa.c`
   * Leitura do arquivo de entrada, validação de dimensões e tokens, alocação dinâmica da matriz da caverna, identificação das coordenadas de início `I` e saída `F`, gerenciamento seguro de memória.
2. **Tomás (Programação Dinâmica):** `include/dp.h`, `src/dp.c`
   * Modelagem de subproblemas e recorrência, preenchimento em ordem topológica reversa da tabela de Programação Dinâmica com memoização, identificação de sobrevivência estrita ($\text{HP} > 0$), registro de predecessores, **Extra 1** (resumo estatístico da caverna com contagem de monstros/poções e visualização formatada) e **Extra 2** (contagem de caminhos ótimos que empatam no HP máximo e exibição didática da tabela DP no terminal) .
3. **Otávio (Reconstrução e Validação do Caminho):** `include/caminho.h`, `src/caminho.c`, `testes/teste_caminho.c`
   * Reconstrução da trajetória ótima $I \to F$ a partir dos predecessores calculados pela DP, validação rigorosa de movimentos, limites e sobrevivência, e geração de relatórios detalhados com mapa em arte ASCII (Extra 3).
4. **Gabs (Execução, Saída, Build e Gerador):** `include/saida.h`, `src/saida.c`, `src/main.c`, `src/gerador.c`, `Makefile`, `testes/teste_saida.c`
   * Interface de linha de comando (CLI) robusta, orquestração dos módulos, gravação estrita de `resultado.txt` (coordenadas ou "impossível"), automação de compilação multiplataforma via `Makefile` e ferramenta independente de geração aleatória e configurável de mapas (Extra 4).
5. **Pedro (Testes Integrados, Desempenho e Revisão):** `testes/` e apoio à integração
   * Concepção dos mapas de teste (casos normais, limites e sem solução), medições experimentais de complexidade e revisão final da consistência dos resultados e do relatório.

---

## Estrutura do Projeto

```text
tp2/
├── include/              # Cabeçalhos públicos (.h) dos módulos
│   ├── mapa.h            # Interface do módulo de mapa
│   ├── dp.h              # Interface do módulo de programação dinâmica
│   ├── caminho.h         # Interface do módulo de caminho e estatísticas
│   └── saida.h           # Interface do módulo de saída (gravação em arquivo)
├── src/                  # Implementações em C (.c)
│   ├── main.c            # Ponto de entrada e coordenação CLI
│   ├── mapa.c            # Leitura e liberação do mapa
│   ├── dp.c              # Algoritmo de programação dinâmica
│   ├── caminho.c         # Reconstrução e validação da rota
│   ├── saida.c           # Gravação e formatação de saída
│   └── gerador.c         # Extra 4: Gerador independente de mapas
├── testes/               # Programas de teste unitário e instâncias de caverna
│   ├── Caverna1.txt      # Exemplo oficial do enunciado (4x5)
│   ├── caminho_escolha.txt # Teste com escolha de caminhos alternativos
│   ├── sem_solucao_morte.txt # Teste de inviabilidade por dano fatal
│   ├── mapa_gerado_pequeno.txt
│   ├── mapa_gerado_medio.txt
│   ├── mapa_gerado_sem_solucao.txt
│   ├── teste_mapa.c      # Teste unitário do módulo mapa
│   ├── teste_caminho.c   # Teste unitário do módulo caminho
│   └── teste_saida.c     # Teste unitário do módulo saída
├── Makefile              # Automação de compilação, testes e limpeza
└── README.md             # Instruções de compilação, execução e documentação
```

---

## Como Compilar

O projeto utiliza um `Makefile` configurado com padrões rigorosos (`-Wall -Wextra -std=c99 -Iinclude`).

### Compilação de todos os alvos (Programa principal, gerador e testes)
No Linux/macOS:
```bash
make all
```
No Windows (com MinGW / MinGW-w64):
```powershell
mingw32-make all
```

Isso gera os seguintes executáveis:
* `tp2` (ou `tp2.exe`): Programa principal integrado.
* `gerador` (ou `gerador.exe`): Ferramenta geradora de mapas da caverna.
* `teste_mapa`, `teste_caminho`, `teste_saida`: Testes unitários isolados de cada módulo.

### Limpeza de binários e arquivos intermediários
```bash
make clean
# ou no Windows:
mingw32-make clean
```

---

## Como Executar o Programa Principal

A sintaxe de execução é:
```bash
./tp2 <arquivo_mapa> [arquivo_saida] [opcoes]
```

### Parâmetros e Opções
* `<arquivo_mapa>`: Caminho do arquivo `.txt` contendo a caverna (obrigatório).
* `[arquivo_saida]`: Caminho do arquivo onde o resultado será gravado (opcional, padrão: `resultado.txt`).
* `-v`, `--verbose`: Exibe relatório passo a passo com evolução do HP e mapa visual ASCII no terminal.
* `-h`, `--help`: Exibe a ajuda da linha de comando.

### Exemplos de Execução

1. **Execução padrão com o mapa do enunciado:**
```bash
./tp2 testes/Caverna1.txt
```
Saída no terminal:
```text
[SUCESSO] Rota encontrada! Passos: 7 | HP final maximo: 50 | Gravado em: resultado.txt
```

2. **Execução detalhada (`--verbose`):**
```bash
./tp2 testes/Caverna1.txt --verbose
```
Além de gravar `resultado.txt`, exibe a tabela passo a passo e o diagrama ASCII da rota escolhida pela programação dinâmica.

3. **Execução com mapa sem solução:**
```bash
./tp2 testes/sem_solucao_morte.txt
```
Saída no terminal:
```text
[SEM SOLUCAO] Nao ha rota viavel ate a saida F. Gravado "impossível" em 'resultado.txt'.
```

---

## Formato do Arquivo de Saída (`resultado.txt`)

O arquivo gravado segue estritamente as regras do trabalho:
* **Quando há solução:** O arquivo contém uma coordenada por linha (formato `linha coluna`, indexação baseada em zero), iniciando exatamente em `I` e terminando em `F`. Exemplo para `Caverna1.txt`:
```text
3 4
2 4
2 3
2 2
2 1
1 1
1 0
```
* **Quando não há solução:** O arquivo contém única e exclusivamente a palavra:
```text
impossível
```

---

## Funcionalidades Extras Desenvolvidas pelo Grupo

### Extra 1: Resumo Estatístico e Visualização da Caverna (Rafaella)
Implementado no módulo `mapa`:
* `mapa_resumo`: Percorre a matriz da caverna e quantifica as salas por categoria: total de monstros com dano acumulado, total de poções com cura acumulada, quantidade de salas neutras, e posições $I$ e $F$.
* `mapa_imprimir`: Exibe a caverna com alinhamento fixo e marcação visual das células de entrada `I` e saída `F`.
* Ativado automaticamente ao rodar o programa principal com a flag `-v` ou `--verbose`.

### Extra 2: Contagem de Caminhos Ótimos e Tabela de DP (Tomás)
Implementado no módulo `dp`:
* **Contagem de Caminhos Ótimos Empatados:** Durante o preenchimento da tabela de memoização, caso ambos os movimentos possíveis (vindo de baixo ou da direita) forneçam exatamente o mesmo HP máximo, o algoritmo soma as trajetórias ótimas viáveis (`solucao.num_caminhos[i][j]`), permitindo identificar se a rota ótima até $F$ é única ou se existem caminhos alternativos de mesmo valor.
* `dp_imprimir_tabela`: Renderiza a matriz de Programação Dinâmica no terminal, indicando o HP ótimo calculado em cada célula alcançável e `-` para células inacessíveis ou de dano letal.
* Integrado ao relatório exibido na opção `--verbose`.

---

## Extra 4: Gerador Independente de Mapas (`gerador`)

O programa `gerador` permite criar de forma parametrizável instâncias de caverna para testes funcionais e de desempenho.

### Sintaxe
```bash
./gerador <arquivo_saida> <linhas> <colunas> <hp_inicial> [val_min] [val_max] [semente] [modo]
```

* `<arquivo_saida>`: Nome do arquivo a ser criado.
* `<linhas>`: Número de linhas da matriz ($L \ge 2$).
* `<colunas>`: Número de colunas da matriz ($C \ge 2$).
* `<hp_inicial>`: HP inicial do estudante ($\text{HP} > 0$).
* `[val_min]`: Menor valor inteiro para células normais (padrão: -20).
* `[val_max]`: Maior valor inteiro para células normais (padrão: +20).
* `[semente]`: Semente pseudoaleatória (garante reprodutibilidade).
* `[modo]`: `soluvel` (garante existência de rota viável) ou `impossivel` (força ausência de rota).

### Exemplo de Uso do Gerador
```bash
./gerador testes/meu_mapa.txt 15 15 50 -15 20 42 soluvel
./tp2 testes/meu_mapa.txt
```

---

## Execução Automatizada de Testes

Para rodar todos os testes de unidade e o teste integrado de ponta a ponta:
```bash
make test
# ou no Windows:
mingw32-make test
```
O comando executa sucessivamente:
1. `teste_mapa` com validação de leitura e tratamento de erros;
2. `teste_caminho` com casos de sucesso, múltiplos caminhos e sem solução;
3. `teste_saida` com verificação de escrita em disco e tratamento defensivo;
4. Execução integrada do binário `tp2` com `Caverna1.txt`.
