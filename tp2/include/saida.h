#ifndef SAIDA_H
#define SAIDA_H

#include <stdio.h>
#include "caminho.h"

// saida.h - Gravacao e exibicao do resultado do percurso

#define SAIDA_OK                  0
#define SAIDA_ERRO_ARQUIVO       -1   // Nao foi possivel abrir ou gravar o arquivo
#define SAIDA_ERRO_PARAMETROS    -2   // Parametros invalidos (ex: ponteiro de arquivo nulo)

// Palavra obrigatoria a ser gravada no resultado.txt caso nao haja solucao
#define SAIDA_TEXTO_SEM_SOLUCAO  "impossível"

/**
 * Grava o resultado final no arquivo especificado (por padrao 'resultado.txt').
 * 
 * Se existe_caminho for 1 e o vetor de caminho for valido:
 *   - Grava cada coordenada do percurso (uma por linha: "linha coluna"),
 *     comecando em I e terminando em F (zero-based).
 * Se existe_caminho for 0:
 *   - Grava exclusivamente a palavra "impossível".
 *
 * Retorna SAIDA_OK em sucesso, ou codigo de erro negativo.
 */
int resultado_gravar(const char *arquivo, int existe_caminho,
                     const Coordenada *caminho, int quantidade);

/**
 * Imprime o resultado formatado em um fluxo de saida aberto (ex: stdout ou stderr).
 * Util para exibicao direta no terminal ou operacoes de log.
 *
 * Retorna SAIDA_OK em sucesso, ou SAIDA_ERRO_PARAMETROS caso stream seja nulo.
 */
int resultado_imprimir(FILE *stream, int existe_caminho,
                       const Coordenada *caminho, int quantidade);

#endif /* SAIDA_H */
