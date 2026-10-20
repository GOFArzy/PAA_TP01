#include "cli.h"

#include <stdio.h>
#include <string.h>

void cli_usage(const char *nome_programa) {
  if (!nome_programa) return;
  fprintf(
      stderr,
      "Uso: %s <arquivo_labirinto.txt> [opcoes]\n"
      "\n"
      "Opcoes:\n"
      "  --globo    Ativa modo globo (labirinto toroidal, bordas se conectam)\n"
      "  --todos    Encontra e exibe todos os caminhos possiveis ate o bau\n"
      "  --analise  Exibe metricas de recursao (chamadas totais e nivel "
      "maximo)\n"
      "  --visual   Visualizador ANSI: imprime o labirinto colorido no "
      "terminal\n"
      "\n"
      "Exemplo:\n"
      "  %s entradas/exemplo.txt --analise\n"
      "  %s entradas/exemplo.txt --globo --visual\n",
      nome_programa, nome_programa, nome_programa);
}

int cli_parse(int argc, char *argv[], Opcoes *opts) {
  if (!opts) return 0;
  opts->arquivo_entrada = NULL;
  opts->modo_globo = 0;
  opts->modo_todos = 0;
  opts->modo_analise = 0;
  opts->modo_visual = 0;

  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "--globo") == 0) {
      opts->modo_globo = 1;
    } else if (strcmp(argv[i], "--todos") == 0) {
      opts->modo_todos = 1;
    } else if (strcmp(argv[i], "--analise") == 0) {
      opts->modo_analise = 1;
    } else if (strcmp(argv[i], "--visual") == 0) {
      opts->modo_visual = 1;
    } else if (argv[i][0] == '-' && argv[i][1] == '-') {
      fprintf(stderr, "Aviso: opcao desconhecida '%s' ignorada.\n", argv[i]);
    } else {
      if (opts->arquivo_entrada == NULL) {
        opts->arquivo_entrada = argv[i];
      } else {
        fprintf(stderr,
                "Aviso: argumento extra '%s' ignorado (arquivo ja definido "
                "como '%s').\n",
                argv[i], opts->arquivo_entrada);
      }
    }
  }

  if (opts->arquivo_entrada == NULL) {
    fprintf(stderr, "Erro: nenhum arquivo de labirinto informado.\n\n");
    cli_usage(argv[0]);
    return 0;
  }

  return 1;
}
