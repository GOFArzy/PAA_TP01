#ifndef CLI_H
#define CLI_H

/* Estrutura que agrupa as configuracoes de execucao passadas via linha de comando */
typedef struct {
  const char *arquivo_entrada; /* Caminho do arquivo .txt do labirinto (obrigatorio) */
  int modo_globo;              /* 1: movimentacao toroidal (--globo) */
  int modo_todos;              /* 1: busca exaustiva de todos os caminhos (--todos) */
  int modo_analise;            /* 1: exibicao de nos e profundidade (--analise) */
  int modo_visual;             /* 1: animacao colorida ANSI no terminal (--visual) */
} Opcoes;

/* Faz o parsing dos argumentos de linha de comando preenchendo opts (1=sucesso, 0=erro) */
int cli_parse(int argc, char *argv[], Opcoes *opts);

/* Exibe manual de uso e opcoes em stderr */
void cli_usage(const char *nome_programa);

#endif
