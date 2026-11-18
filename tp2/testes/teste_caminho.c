#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mapa.h"
#include "dp.h"
#include "caminho.h"

// Teste de caso de sucesso
static void testar_caso_sucesso(const char *arquivo)
{
    Mapa m;
    int r = mapa_ler(arquivo, &m);
    assert(r == MAPA_OK);

    Solucao s;
    int ok_dp = dp_resolver(&m, &s);
    assert(ok_dp == 1);
    assert(s.existe_caminho == 1);

    Coordenada *caminho = NULL;
    int qtd = 0;
    int res_rec = caminho_reconstruir(&m, &s, &caminho, &qtd);
    assert(res_rec == CAMINHO_OK);
    assert(caminho != NULL);
    assert(qtd > 0);

    printf("[%s] Caminho reconstruido: %d passos (HP final = %d)\n",
           arquivo, qtd, s.hp_final);
    for (int i = 0; i < qtd; i++)
        printf("  %d %d\n", caminho[i].linha, caminho[i].coluna);

    int res_val = caminho_validar(&m, caminho, qtd);
    assert(res_val == CAMINHO_OK);

    // Demonstra funcoes do Extra 3
    caminho_imprimir_relatorio(stdout, &m, caminho, qtd);
    caminho_desenhar_ascii(stdout, &m, caminho, qtd);

    caminho_liberar(caminho);
    dp_liberar(&m, &s);
    mapa_liberar(&m);
}

// Teste de caso sem solucao
static void testar_caso_sem_solucao(const char *arquivo)
{
    Mapa m;
    int r = mapa_ler(arquivo, &m);
    assert(r == MAPA_OK);

    Solucao s;
    int ok_dp = dp_resolver(&m, &s);
    assert(ok_dp == 0);
    assert(s.existe_caminho == 0);

    Coordenada *caminho = NULL;
    int qtd = 0;
    int res_rec = caminho_reconstruir(&m, &s, &caminho, &qtd);
    assert(res_rec == CAMINHO_SEM_SOLUCAO);
    assert(caminho == NULL);
    assert(qtd == 0);

    printf("[%s] Sem solucao confirmado com sucesso\n", arquivo);

    dp_liberar(&m, &s);
    mapa_liberar(&m);
}

// Teste de validacoes negativas de caminho_validar
static void testar_validacoes_negativas(void)
{
    Mapa m;
    int r = mapa_ler("testes/Caverna1.txt", &m);
    assert(r == MAPA_OK);

    // Parametros invalidos
    assert(caminho_validar(NULL, NULL, 0) == CAMINHO_ERRO_PARAMETROS);

    // Inicio errado
    Coordenada c_inicio_errado[] = {{0, 0}, {1, 0}};
    assert(caminho_validar(&m, c_inicio_errado, 2) == CAMINHO_INVALIDO_INICIO);

    // Fim errado
    Coordenada c_fim_errado[] = {
        {m.inicio_linha, m.inicio_coluna},
        {m.inicio_linha - 1, m.inicio_coluna}
    };
    assert(caminho_validar(&m, c_fim_errado, 2) == CAMINHO_INVALIDO_FIM);

    // Movimento ilegal
    Coordenada c_mov_ilegal[] = {
        {m.inicio_linha, m.inicio_coluna},
        {m.inicio_linha - 1, m.inicio_coluna - 1},
        {m.fim_linha, m.fim_coluna}
    };
    assert(caminho_validar(&m, c_mov_ilegal, 3) == CAMINHO_INVALIDO_MOVIMENTO);

    // Coordenada fora de limite
    Coordenada c_limites[] = {
        {m.inicio_linha, m.inicio_coluna},
        {-1, m.inicio_coluna},
        {m.fim_linha, m.fim_coluna}
    };
    assert(caminho_validar(&m, c_limites, 3) == CAMINHO_INVALIDO_LIMITES ||
           caminho_validar(&m, c_limites, 3) == CAMINHO_INVALIDO_MOVIMENTO);

    printf("[Validacoes negativas] Todos os casos de rejeicao funcionaram\n");

    mapa_liberar(&m);
}

int main(void)
{
    testar_caso_sucesso("testes/Caverna1.txt");
    testar_caso_sucesso("testes/caminho_escolha.txt");
    testar_caso_sem_solucao("testes/sem_solucao_morte.txt");
    testar_validacoes_negativas();

    printf("\nTodos os testes de caminho passaram com sucesso!\n");
    return 0;
}
