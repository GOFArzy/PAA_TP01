#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "saida.h"
#include "caminho.h"

// Testa gravacao de caminho existente
static void testar_gravacao_sucesso(void)
{
    const char *arquivo_teste = "testes/temp_saida_sucesso.txt";
    Coordenada rota[] = {
        {3, 4},
        {2, 4},
        {2, 3},
        {1, 1},
        {1, 0}
    };
    int qtd = 5;

    int r = resultado_gravar(arquivo_teste, 1, rota, qtd);
    assert(r == SAIDA_OK);

    // Le o arquivo gerado e verifica conteudo
    FILE *f = fopen(arquivo_teste, "r");
    assert(f != NULL);

    char linha[64];
    int lidos = 0;
    while (fgets(linha, sizeof(linha), f) != NULL) {
        int lin, col;
        int parsed = sscanf(linha, "%d %d", &lin, &col);
        assert(parsed == 2);
        assert(lin == rota[lidos].linha);
        assert(col == rota[lidos].coluna);
        lidos++;
    }
    assert(lidos == qtd);
    fclose(f);

    remove(arquivo_teste);
    printf("[teste_saida] Gravacao de caminho com sucesso validada.\n");
}

// Testa gravacao quando nao ha caminho
static void testar_gravacao_sem_solucao(void)
{
    const char *arquivo_teste = "testes/temp_saida_sem_solucao.txt";

    int r = resultado_gravar(arquivo_teste, 0, NULL, 0);
    assert(r == SAIDA_OK);

    FILE *f = fopen(arquivo_teste, "r");
    assert(f != NULL);

    char buffer[128];
    char *lido = fgets(buffer, sizeof(buffer), f);
    assert(lido != NULL);

    // Remove newline para comparar
    buffer[strcspn(buffer, "\r\n")] = '\0';
    assert(strcmp(buffer, SAIDA_TEXTO_SEM_SOLUCAO) == 0);

    // Nao deve ter nenhuma linha adicional
    assert(fgets(buffer, sizeof(buffer), f) == NULL);

    fclose(f);
    remove(arquivo_teste);
    printf("[teste_saida] Gravacao de 'impossível' sem solucao validada.\n");
}

// Testa tratamento de erros de parametros
static void testar_erros_parametros(void)
{
    assert(resultado_gravar(NULL, 1, NULL, 0) == SAIDA_ERRO_PARAMETROS);
    assert(resultado_imprimir(NULL, 1, NULL, 0) == SAIDA_ERRO_PARAMETROS);
    printf("[teste_saida] Tratamento defensivo de parametros validado.\n");
}

int main(void)
{
    printf("Iniciando testes do modulo de saida (saida.c)...\n");
    testar_gravacao_sucesso();
    testar_gravacao_sem_solucao();
    testar_erros_parametros();
    printf("Todos os testes do modulo de saida passaram com sucesso!\n");
    return 0;
}
