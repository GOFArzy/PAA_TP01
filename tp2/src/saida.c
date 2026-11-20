#include <stdio.h>
#include <stdlib.h>
#include "saida.h"

// Imprime o resultado formatado em um fluxo de saida aberto
int resultado_imprimir(FILE *stream, int existe_caminho,
                       const Coordenada *caminho, int quantidade)
{
    if (stream == NULL)
        return SAIDA_ERRO_PARAMETROS;

    // Caso nao haja caminho viavel
    if (!existe_caminho || caminho == NULL || quantidade <= 0) {
        fprintf(stream, "%s\n", SAIDA_TEXTO_SEM_SOLUCAO);
        return SAIDA_OK;
    }

    // Caso exista caminho: imprime uma coordenada por linha (linha coluna)
    for (int i = 0; i < quantidade; i++) {
        fprintf(stream, "%d %d\n", caminho[i].linha, caminho[i].coluna);
    }

    return SAIDA_OK;
}

// Grava o resultado final no arquivo especificado (resultado.txt)
int resultado_gravar(const char *arquivo, int existe_caminho,
                     const Coordenada *caminho, int quantidade)
{
    if (arquivo == NULL)
        return SAIDA_ERRO_PARAMETROS;

    FILE *f = fopen(arquivo, "w");
    if (f == NULL)
        return SAIDA_ERRO_ARQUIVO;

    int status = resultado_imprimir(f, existe_caminho, caminho, quantidade);

    if (fclose(f) != 0)
        return SAIDA_ERRO_ARQUIVO;

    return status;
}
