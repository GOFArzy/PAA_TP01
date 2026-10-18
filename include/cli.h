#ifndef CLI_H
#define CLI_H

/*
 * Estrutura que agrupa todas as opcoes de linha de comando aceitas pelo
 * programa.
 *
 * Campos:
 *   arquivo_entrada  - caminho para o arquivo .txt do labirinto (obrigatorio).
 *   modo_globo       - 1 se globo foi passado (movimento toroidal).
 *   modo_todos       - 1 se todos foi passado (encontrar todos os caminhos).
 *   modo_analise     - 1 se analise foi passado (exibir metricas de recursao).
 *   modo_visual      - 1 se visual foi passado (visualizador ANSI no terminal).
 */
typedef struct {
  const char *arquivo_entrada;
  int modo_globo;
  int modo_todos;
  int modo_analise;
  int modo_visual;
} Opcoes;

/*
 * Faz o parse dos argumentos de linha de comando em argv e preenche *opts.
 *
 * Retorna 1 em caso de sucesso.
 * Retorna 0 se os argumentos forem invalidos (ex: nenhum arquivo informado),
 * imprimindo a mensagem de uso correta em stderr.
 *
 * Uso esperado:
 *   ./tp01 <arquivo_labirinto.txt> [globo] [todos] [analise] [visual]
 */
int cli_parse(int argc, char *argv[], Opcoes *opts);

// Imprime a mensagem de uso do programa em stderr.
// Chamada automaticamente por cli_parse em caso de erro.
void cli_usage(const char *nome_programa);

#endif /* CLI_H */
