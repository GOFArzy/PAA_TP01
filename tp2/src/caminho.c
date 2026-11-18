#include <stdio.h>
#include <stdlib.h>
#include "caminho.h"
#include "dp.h"
#include "mapa.h"

// Reconstroi o caminho I -> F a partir dos predecessores da DP
int caminho_reconstruir(const Mapa *mapa, const Solucao *solucao,
                        Coordenada **caminho, int *quantidade)
{
    if (mapa == NULL || solucao == NULL || caminho == NULL || quantidade == NULL)
        return CAMINHO_ERRO_PARAMETROS;

    *caminho = NULL;
    *quantidade = 0;

    // Se nao ha solucao valida encontrada pela DP
    if (!solucao->existe_caminho || solucao->pai == NULL || solucao->melhor_hp == NULL)
        return CAMINHO_SEM_SOLUCAO;

    int il = mapa->inicio_linha;
    int ic = mapa->inicio_coluna;
    int fl = mapa->fim_linha;
    int fc = mapa->fim_coluna;

    if (fl < 0 || fl >= mapa->linhas || fc < 0 || fc >= mapa->colunas ||
        solucao->melhor_hp[fl][fc] <= 0) {
        return CAMINHO_SEM_SOLUCAO;
    }

    // Conta o numero de passos de F ate I
    int max_passos = mapa->linhas + mapa->colunas + 2;
    int cont = 1;
    Coordenada atual = {fl, fc};

    while (atual.linha != il || atual.coluna != ic) {
        if (atual.linha < 0 || atual.linha >= mapa->linhas ||
            atual.coluna < 0 || atual.coluna >= mapa->colunas) {
            return CAMINHO_SEM_SOLUCAO;
        }

        Coordenada anterior = solucao->pai[atual.linha][atual.coluna];
        if (anterior.linha == -1 && anterior.coluna == -1)
            return CAMINHO_SEM_SOLUCAO;

        atual = anterior;
        cont++;
        if (cont > max_passos)
            return CAMINHO_SEM_SOLUCAO;
    }

    // Aloca vetor de coordenadas
    Coordenada *vet = (Coordenada *)malloc((size_t)cont * sizeof(Coordenada));
    if (vet == NULL)
        return CAMINHO_ERRO_MEMORIA;

    // Preenche de tras para frente para manter a ordem I -> F
    atual.linha = fl;
    atual.coluna = fc;
    int idx = cont - 1;
    vet[idx] = atual;

    while (atual.linha != il || atual.coluna != ic) {
        atual = solucao->pai[atual.linha][atual.coluna];
        idx--;
        vet[idx] = atual;
    }

    *caminho = vet;
    *quantidade = cont;
    return CAMINHO_OK;
}

// Valida movimentos, limites e sobrevivencia
int caminho_validar(const Mapa *mapa, const Coordenada *caminho, int quantidade)
{
    if (mapa == NULL || caminho == NULL || quantidade <= 0)
        return CAMINHO_ERRO_PARAMETROS;

    // Comeca em I
    if (caminho[0].linha != mapa->inicio_linha || caminho[0].coluna != mapa->inicio_coluna)
        return CAMINHO_INVALIDO_INICIO;

    // Termina em F
    if (caminho[quantidade - 1].linha != mapa->fim_linha ||
        caminho[quantidade - 1].coluna != mapa->fim_coluna)
        return CAMINHO_INVALIDO_FIM;

    int hp_atual = mapa->hp_inicial;
    if (hp_atual <= 0)
        return CAMINHO_INVALIDO_MORTE;

    if (caminho[0].linha < 0 || caminho[0].linha >= mapa->linhas ||
        caminho[0].coluna < 0 || caminho[0].coluna >= mapa->colunas)
        return CAMINHO_INVALIDO_LIMITES;

    // Valida os passos subsequentes
    for (int k = 1; k < quantidade; k++) {
        int l_ant = caminho[k - 1].linha;
        int c_ant = caminho[k - 1].coluna;
        int l_atu = caminho[k].linha;
        int c_atu = caminho[k].coluna;

        // Limites da celula
        if (l_atu < 0 || l_atu >= mapa->linhas || c_atu < 0 || c_atu >= mapa->colunas)
            return CAMINHO_INVALIDO_LIMITES;

        // Movimento: apenas cima (-1 lin) ou esquerda (-1 col)
        int dl = l_atu - l_ant;
        int dc = c_atu - c_ant;
        int eh_cima = (dl == -1 && dc == 0);
        int eh_esquerda = (dl == 0 && dc == -1);

        if (!eh_cima && !eh_esquerda)
            return CAMINHO_INVALIDO_MOVIMENTO;

        // Efeito da celula
        hp_atual += mapa->valor[l_atu][c_atu];
        if (hp_atual <= 0)
            return CAMINHO_INVALIDO_MORTE;
    }

    return CAMINHO_OK;
}

// Libera memoria do caminho
void caminho_liberar(Coordenada *caminho)
{
    if (caminho != NULL)
        free(caminho);
}

// Extra 3: calcula metricas do percurso
int caminho_calcular_estatisticas(const Mapa *mapa, const Coordenada *caminho,
                                 int quantidade, EstatisticasCaminho *estat)
{
    if (mapa == NULL || caminho == NULL || quantidade <= 0 || estat == NULL)
        return CAMINHO_ERRO_PARAMETROS;

    estat->passos = quantidade;
    estat->hp_inicial = mapa->hp_inicial;
    estat->hp_minimo = mapa->hp_inicial;
    estat->hp_maximo = mapa->hp_inicial;
    estat->dano_total = 0;
    estat->cura_total = 0;

    int hp = mapa->hp_inicial;

    for (int k = 1; k < quantidade; k++) {
        int l = caminho[k].linha;
        int c = caminho[k].coluna;
        int val = mapa->valor[l][c];

        hp += val;
        if (val < 0)
            estat->dano_total += (-val);
        else if (val > 0)
            estat->cura_total += val;

        if (hp < estat->hp_minimo)
            estat->hp_minimo = hp;
        if (hp > estat->hp_maximo)
            estat->hp_maximo = hp;
    }

    estat->hp_final = hp;
    return CAMINHO_OK;
}

// Extra 3: relatorio passo a passo
void caminho_imprimir_relatorio(FILE *saida, const Mapa *mapa,
                               const Coordenada *caminho, int quantidade)
{
    if (saida == NULL || mapa == NULL || caminho == NULL || quantidade <= 0)
        return;

    EstatisticasCaminho est;
    caminho_calcular_estatisticas(mapa, caminho, quantidade, &est);

    fprintf(saida, "\n=== Relatorio Detalhado do Percurso (Extra 3) ===\n");
    fprintf(saida, "Passo | Coordenada | Movimento | Efeito  | HP Resultante\n");
    fprintf(saida, "------+------------+-----------+---------+--------------\n");

    int hp = mapa->hp_inicial;
    fprintf(saida, "%5d |   (%2d, %2d) |  INICIO   |    --   | %5d [VIVO]\n",
            0, caminho[0].linha, caminho[0].coluna, hp);

    for (int k = 1; k < quantidade; k++) {
        int l_ant = caminho[k - 1].linha;
        int l_atu = caminho[k].linha;
        int c_atu = caminho[k].coluna;
        int val = mapa->valor[l_atu][c_atu];

        const char *mov = (l_atu == l_ant - 1) ? " CIMA    " : "ESQUERDA ";
        hp += val;

        char efeito_str[16];
        if (k == quantidade - 1)
            snprintf(efeito_str, sizeof(efeito_str), "F (0)");
        else if (val > 0)
            snprintf(efeito_str, sizeof(efeito_str), "+%d", val);
        else if (val < 0)
            snprintf(efeito_str, sizeof(efeito_str), "%d", val);
        else
            snprintf(efeito_str, sizeof(efeito_str), "0");

        fprintf(saida, "%5d |   (%2d, %2d) | %s | %7s | %5d [%s]\n",
                k, l_atu, c_atu, mov, efeito_str, hp, (hp > 0) ? "VIVO" : "MORTO");
    }

    fprintf(saida, "------+------------+-----------+---------+--------------\n");
    fprintf(saida, "Passos: %d | HP: %d -> %d (Min: %d, Max: %d) | Dano: %d | Cura: %d\n\n",
            est.passos, est.hp_inicial, est.hp_final, est.hp_minimo, est.hp_maximo,
            est.dano_total, est.cura_total);
}

// Extra 3: visualizacao da rota em ASCII
void caminho_desenhar_ascii(FILE *saida, const Mapa *mapa,
                            const Coordenada *caminho, int quantidade)
{
    if (saida == NULL || mapa == NULL)
        return;

    fprintf(saida, "=== Visualizacao ASCII da Rota (Extra 3) ===\n");

    // Matriz de caracteres para desenhar o percurso
    char **grid = (char **)malloc((size_t)mapa->linhas * sizeof(char *));
    if (grid == NULL)
        return;

    for (int i = 0; i < mapa->linhas; i++) {
        grid[i] = (char *)malloc((size_t)mapa->colunas * sizeof(char));
        if (grid[i] == NULL) {
            for (int k = 0; k < i; k++)
                free(grid[k]);
            free(grid);
            return;
        }
        for (int j = 0; j < mapa->colunas; j++)
            grid[i][j] = '.';
    }

    if (caminho != NULL && quantidade > 0) {
        for (int k = 0; k < quantidade - 1; k++) {
            int l_atu = caminho[k].linha;
            int c_atu = caminho[k].coluna;
            int l_prox = caminho[k + 1].linha;
            int c_prox = caminho[k + 1].coluna;

            if (k == 0)
                grid[l_atu][c_atu] = 'I';
            else if (l_prox == l_atu - 1)
                grid[l_atu][c_atu] = '^';
            else if (c_prox == c_atu - 1)
                grid[l_atu][c_atu] = '<';
            else
                grid[l_atu][c_atu] = '*';
        }
        int fl = caminho[quantidade - 1].linha;
        int fc = caminho[quantidade - 1].coluna;
        grid[fl][fc] = 'F';
    }

    // Cabecalho das colunas
    fprintf(saida, "    ");
    for (int j = 0; j < mapa->colunas; j++)
        fprintf(saida, " %2d ", j);
    fprintf(saida, "\n    ");
    for (int j = 0; j < mapa->colunas; j++)
        fprintf(saida, "----");
    fprintf(saida, "\n");

    // Linhas
    for (int i = 0; i < mapa->linhas; i++) {
        fprintf(saida, "%2d |", i);
        for (int j = 0; j < mapa->colunas; j++) {
            char ch = grid[i][j];
            if (ch == 'I' || ch == 'F')
                fprintf(saida, " [%c]", ch);
            else if (ch == '^' || ch == '<' || ch == '*')
                fprintf(saida, "  %c ", ch);
            else
                fprintf(saida, "  . ");
        }
        fprintf(saida, " |\n");
    }

    fprintf(saida, "    ");
    for (int j = 0; j < mapa->colunas; j++)
        fprintf(saida, "----");
    fprintf(saida, "\n");
    fprintf(saida, "Legenda: [I] Inicio | [F] Destino | ^ Cima | < Esquerda | . Nao visitado\n\n");

    for (int i = 0; i < mapa->linhas; i++)
        free(grid[i]);
    free(grid);
}
