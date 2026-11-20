#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// gerador.c - Extra 4: Gerador independente de arquivos de mapa da caverna

static void exibir_ajuda_gerador(const char *prog)
{
    printf("=================================================================\n");
    printf("  Extra 4: Gerador de Arquivos de Mapa para o TP02 - PAA\n");
    printf("=================================================================\n\n");
    printf("Uso: %s <arquivo_saida> <linhas> <colunas> <hp_inicial> [val_min] [val_max] [semente] [modo]\n\n", prog);
    printf("Parametros:\n");
    printf("  <arquivo_saida>  Nome do arquivo .txt a ser gerado\n");
    printf("  <linhas>         Numero de linhas da caverna (L >= 2)\n");
    printf("  <colunas>        Numero de colunas da caverna (C >= 2)\n");
    printf("  <hp_inicial>     Pontos de vida iniciais do personagem (HP > 0)\n");
    printf("  [val_min]        Menor valor para celulas numericas (padrao: -20)\n");
    printf("  [val_max]        Maior valor para celulas numericas (padrao: +20)\n");
    printf("  [semente]        Semente pseudoaleatoria para reproducao (padrao: tempo atual)\n");
    printf("  [modo]           'soluvel' (garante rota viavel) ou 'impossivel' (padrao: soluvel)\n\n");
    printf("Exemplo:\n");
    printf("  %s testes/mapa_medio.txt 10 10 50 -15 25 1234 soluvel\n", prog);
}

int main(int argc, char *argv[])
{
    if (argc < 5) {
        if (argc >= 2 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0)) {
            exibir_ajuda_gerador(argv[0]);
            return EXIT_SUCCESS;
        }
        fprintf(stderr, "Erro: Parametros insuficientes.\n");
        exibir_ajuda_gerador(argv[0]);
        return EXIT_FAILURE;
    }

    const char *arquivo_saida = argv[1];
    int linhas = atoi(argv[2]);
    int colunas = atoi(argv[3]);
    int hp_inicial = atoi(argv[4]);

    if (linhas < 2 || colunas < 2 || hp_inicial <= 0) {
        fprintf(stderr, "Erro: Dimensoes devem ser >= 2 e HP inicial deve ser > 0.\n");
        return EXIT_FAILURE;
    }

    int val_min = (argc > 5) ? atoi(argv[5]) : -20;
    int val_max = (argc > 6) ? atoi(argv[6]) : 20;
    if (val_min > val_max) {
        int tmp = val_min;
        val_min = val_max;
        val_max = tmp;
    }

    unsigned int semente = (argc > 7) ? (unsigned int)strtoul(argv[7], NULL, 10) : (unsigned int)time(NULL);
    const char *modo = (argc > 8) ? argv[8] : "soluvel";
    int eh_impossivel = (strcmp(modo, "impossivel") == 0 || strcmp(modo, "imp") == 0);

    srand(semente);

    // Aloca matriz de celulas em memoria (armazenaremos como int, com sentinelas para I e F)
    #define CELULA_I  (1000000001)
    #define CELULA_F  (1000000002)

    int **matriz = (int **)malloc((size_t)linhas * sizeof(int *));
    if (matriz == NULL) {
        fprintf(stderr, "Erro: Falha de alocacao de memoria para o gerador.\n");
        return EXIT_FAILURE;
    }
    for (int i = 0; i < linhas; i++) {
        matriz[i] = (int *)malloc((size_t)colunas * sizeof(int));
        if (matriz[i] == NULL) {
            for (int k = 0; k < i; k++) free(matriz[k]);
            free(matriz);
            fprintf(stderr, "Erro: Falha de alocacao de memoria para o gerador.\n");
            return EXIT_FAILURE;
        }
    }

    // Preenche com valores aleatorios no intervalo [val_min, val_max]
    int intervalo = (val_max - val_min + 1);
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            matriz[i][j] = val_min + (rand() % intervalo);
        }
    }

    int inicio_l, inicio_c;
    int fim_l, fim_c;

    if (eh_impossivel) {
        // Modo sem solucao: posiciona I no topo/esquerda e F embaixo/direita,
        // tornando F inalcancavel apenas com movimentos para cima e esquerda,
        // OU cerca I com monstros fatais (-9999).
        inicio_l = 0;
        inicio_c = 0;
        fim_l = linhas - 1;
        fim_c = colunas - 1;

        matriz[inicio_l][inicio_c] = CELULA_I;
        matriz[fim_l][fim_c] = CELULA_F;

        // Adiciona monstros fatais em todas as celulas para garantir morte caso alguem tente outra rota
        for (int i = 0; i < linhas; i++) {
            for (int j = 0; j < colunas; j++) {
                if (matriz[i][j] != CELULA_I && matriz[i][j] != CELULA_F) {
                    matriz[i][j] = -(hp_inicial + 50);
                }
            }
        }
    } else {
        // Modo soluvel:
        // Posiciona I no canto inferior direito e F no canto superior esquerdo
        inicio_l = linhas - 1;
        inicio_c = colunas - 1;
        fim_l = 0;
        fim_c = 0;

        matriz[inicio_l][inicio_c] = CELULA_I;
        matriz[fim_l][fim_c] = CELULA_F;

        // Garante ao menos um caminho viavel entre I e F
        // Traca um caminho de I ate F movendo aleatoriamente para cima ou esquerda
        int cur_l = inicio_l;
        int cur_c = inicio_c;
        while (cur_l > fim_l || cur_c > fim_c) {
            int pode_subir = (cur_l > fim_l);
            int pode_esq = (cur_c > fim_c);

            if (pode_subir && pode_esq) {
                if (rand() % 2 == 0)
                    cur_l--;
                else
                    cur_c--;
            } else if (pode_subir) {
                cur_l--;
            } else {
                cur_c--;
            }

            // Se nao for F, coloca um valor seguro (ex: cura de +5 ou monstro leve que nao mata)
            if (!(cur_l == fim_l && cur_c == fim_c)) {
                matriz[cur_l][cur_c] = rand() % 15; // valor entre 0 e +14 (positivo/neutro)
            }
        }
    }

    // Grava no arquivo no formato especificado
    FILE *f = fopen(arquivo_saida, "w");
    if (f == NULL) {
        fprintf(stderr, "Erro ao criar arquivo '%s'.\n", arquivo_saida);
        for (int i = 0; i < linhas; i++) free(matriz[i]);
        free(matriz);
        return EXIT_FAILURE;
    }

    // Primeira linha: L C HP
    fprintf(f, "%d %d %d\n", linhas, colunas, hp_inicial);

    // Linhas seguintes: valores separados por espaco
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (matriz[i][j] == CELULA_I) {
                fprintf(f, "I");
            } else if (matriz[i][j] == CELULA_F) {
                fprintf(f, "F");
            } else {
                if (matriz[i][j] > 0)
                    fprintf(f, "+%d", matriz[i][j]);
                else
                    fprintf(f, "%d", matriz[i][j]);
            }

            if (j + 1 < colunas)
                fprintf(f, " ");
        }
        fprintf(f, "\n");
    }

    fclose(f);

    // Libera memoria
    for (int i = 0; i < linhas; i++) free(matriz[i]);
    free(matriz);

    printf("[GERADOR] Mapa gerado com sucesso em '%s':\n", arquivo_saida);
    printf("  Dimensoes: %d x %d | HP inicial: %d\n", linhas, colunas, hp_inicial);
    printf("  Intervalo: [%d, %d] | Semente: %u | Modo: %s\n",
           val_min, val_max, semente, eh_impossivel ? "impossivel" : "soluvel");

    return EXIT_SUCCESS;
}
