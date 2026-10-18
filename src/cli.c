/*
 * cli.c
 *
 * Modulo de Interface de Linha de Comando (CLI).
 *
 * Responsabilidades:
 *  - Fazer o parse dos argumentos passados a main() via argc/argv.
 *  - Preencher a struct Opcoes com os valores lidos.
 *  - Validar que o arquivo de entrada foi informado.
 *  - Imprimir mensagem de uso em caso de erro.
 *
 * Flags suportadas (podem aparecer em qualquer ordem apos o arquivo):
 *  --globo    Ativa movimento toroidal (Modo Globo, Extra 3).
 *  --todos    Ativa busca por todos os caminhos possiveis (Extra 2).
 *  --analise  Ativa exibicao de metricas de recursao (chamadas e nivel max).
 *  --visual   Ativa o visualizador ANSI colorido no terminal (Extra 4).
 *
 * Exemplo de uso:
 *  ./tp01 entrada.txt --globo --analise
 *  ./tp01 entrada.txt --todos --visual
 */

#include "cli.h"

#include <stdio.h>
#include <string.h>

// cli_usage()

// Imprime no stderr a sintaxe correta de uso do programa.

void cli_usage(const char *nome_programa) {
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
      "  %s entrada.txt --analise\n"
      "  %s entrada.txt --globo --visual\n",
      nome_programa, nome_programa, nome_programa);
}

/*
 * cli_parse()
 *
 * Percorre argv buscando o arquivo de entrada e as flags opcionais.
 *
 * Regras:
 *  - O primeiro argumento que NAO comeca com "--" e tratado como arquivo.
 *  - Flags desconhecidas sao avisadas em stderr mas NAO abortam o programa.
 *  - Se nenhum arquivo for encontrado, imprime uso e retorna 0.
 *
 * Retorna 1 em caso de sucesso, 0 em caso de erro.
 */
int cli_parse(int argc, char *argv[], Opcoes *opts) {
  // Inicializa todos os campos com valores padrao
  opts->arquivo_entrada = NULL;
  opts->modo_globo = 0;
  opts->modo_todos = 0;
  opts->modo_analise = 0;
  opts->modo_visual = 0;

  // Percorre os argumentos a partir de argv[1] (argv[0] e o nome do prog.)
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
      // Flag desconhecida: avisa mas continua
      fprintf(stderr, "Aviso: opcao desconhecida '%s' ignorada.\n", argv[i]);

    } else {
      // Primeiro argumento sem '--' e o arquivo de entrada
      if (opts->arquivo_entrada == NULL) {
        opts->arquivo_entrada = argv[i];
      } else {
        fprintf(stderr,
                "Aviso: argumento extra '%s' ignorado "
                "(arquivo de entrada ja definido como '%s').\n",
                argv[i], opts->arquivo_entrada);
      }
    }
  }

  // Valida: arquivo de entrada e obrigatorio
  if (opts->arquivo_entrada == NULL) {
    fprintf(stderr, "Erro: nenhum arquivo de labirinto informado.\n\n");
    cli_usage(argv[0]);
    return 0;
  }

  return 1;
}
