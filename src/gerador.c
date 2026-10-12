#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

static const int DX[4] = {-1, 1, 0, 0};
static const int DY[4] = {0, 0, -1, 1};

static void embaralhar(int *arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }
}

static int dfs_caminho(int **visitado, int linhas, int colunas,
                       int lin, int col,
                       int dest_lin, int dest_col,
                       int *caminho_lin, int *caminho_col,
                       int *tam_caminho) {
    visitado[lin][col] = 1;
    caminho_lin[*tam_caminho] = lin;
    caminho_col[*tam_caminho] = col;
    (*tam_caminho)++;

    if (lin == dest_lin && col == dest_col) {
        return 1;
    }

    int ordem[4] = {0, 1, 2, 3};
    embaralhar(ordem, 4);

    for (int i = 0; i < 4; i++) {
        int nl = lin + DX[ordem[i]];
        int nc = col + DY[ordem[i]];

        if (nl >= 0 && nl < linhas && nc >= 0 && nc < colunas && !visitado[nl][nc]) {
            if (dfs_caminho(visitado, linhas, colunas, nl, nc,
                            dest_lin, dest_col,
                            caminho_lin, caminho_col, tam_caminho)) {
                return 1;
            }
        }
    }

    (*tam_caminho)--;
    return 0;
}

static void uso(const char *prog) {
    fprintf(stderr, "Uso: %s <linhas> <colunas> <num_chaves> <saida.txt> [seed] [prob_paredes]\n", prog);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        uso(argv[0]);
        return 1;
    }

    int linhas = atoi(argv[1]);
    int colunas = atoi(argv[2]);
    int num_chaves = atoi(argv[3]);
    const char *arquivo_saida = argv[4];
    unsigned int seed = (unsigned int)time(NULL);
    int prob_paredes = 35;

    if (argc >= 6) seed = (unsigned int)atoi(argv[5]);
    if (argc >= 7) prob_paredes = atoi(argv[6]);

    if (linhas < 2 || colunas < 2) {
        fprintf(stderr, "Erro: dimensoes minimas sao 2x2.\n");
        return 1;
    }
    if (num_chaves < 0) {
        fprintf(stderr, "Erro: numero de chaves deve ser nao-negativo.\n");
        return 1;
    }
    if (prob_paredes < 0 || prob_paredes > 100) {
        fprintf(stderr, "Erro: probabilidade de paredes deve ser entre 0 e 100.\n");
        return 1;
    }

    srand(seed);

    char **grade = (char **)malloc(linhas * sizeof(char *));
    if (!grade) {
        fprintf(stderr, "Erro de alocacao de memoria.\n");
        return 1;
    }
    for (int i = 0; i < linhas; i++) {
        grade[i] = (char *)malloc(colunas * sizeof(char));
        if (!grade[i]) {
            fprintf(stderr, "Erro de alocacao de memoria.\n");
            for (int k = 0; k < i; k++) free(grade[k]);
            free(grade);
            return 1;
        }
        for (int j = 0; j < colunas; j++) {
            grade[i][j] = '0';
        }
    }

    int inicio_lin = 0, inicio_col = 0;
    int bau_lin = linhas - 1, bau_col = colunas - 1;

    grade[inicio_lin][inicio_col] = 'A';
    grade[bau_lin][bau_col] = 'X';

    int **visitado = (int **)malloc(linhas * sizeof(int *));
    for (int i = 0; i < linhas; i++) {
        visitado[i] = (int *)calloc(colunas, sizeof(int));
    }

    int max_celulas = linhas * colunas;
    int *caminho_lin = (int *)malloc(max_celulas * sizeof(int));
    int *caminho_col = (int *)malloc(max_celulas * sizeof(int));
    int tam_caminho = 0;

    int sucesso = dfs_caminho(visitado, linhas, colunas,
                              inicio_lin, inicio_col,
                              bau_lin, bau_col,
                              caminho_lin, caminho_col, &tam_caminho);

    if (!sucesso) {
        fprintf(stderr, "Erro ao gerar caminho inicial.\n");
        for (int i = 0; i < linhas; i++) {
            free(grade[i]);
            free(visitado[i]);
        }
        free(grade);
        free(visitado);
        free(caminho_lin);
        free(caminho_col);
        return 1;
    }

    int **no_caminho = (int **)malloc(linhas * sizeof(int *));
    for (int i = 0; i < linhas; i++) {
        no_caminho[i] = (int *)calloc(colunas, sizeof(int));
    }
    for (int k = 0; k < tam_caminho; k++) {
        no_caminho[caminho_lin[k]][caminho_col[k]] = 1;
    }

    int num_possiveis = tam_caminho - 2;
    if (num_possiveis < 0) num_possiveis = 0;

    if (num_chaves > num_possiveis) {
        num_chaves = num_possiveis;
    }

    if (num_chaves > 0) {
        int *indices = (int *)malloc(num_possiveis * sizeof(int));
        for (int k = 0; k < num_possiveis; k++) {
            indices[k] = k + 1;
        }
        embaralhar(indices, num_possiveis);

        for (int k = 0; k < num_chaves; k++) {
            int idx = indices[k];
            grade[caminho_lin[idx]][caminho_col[idx]] = 'C';
        }

        free(indices);
    }

    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (!no_caminho[i][j] && grade[i][j] == '0') {
                if (rand() % 100 < prob_paredes) {
                    grade[i][j] = '1';
                }
            }
        }
    }

    FILE *fp = fopen(arquivo_saida, "w");
    if (!fp) {
        fprintf(stderr, "Erro ao abrir arquivo '%s' para escrita.\n", arquivo_saida);
        for (int i = 0; i < linhas; i++) {
            free(grade[i]);
            free(visitado[i]);
            free(no_caminho[i]);
        }
        free(grade);
        free(visitado);
        free(no_caminho);
        free(caminho_lin);
        free(caminho_col);
        return 1;
    }

    fprintf(fp, "%d %d\n", linhas, colunas);
    fprintf(fp, "%d\n", num_chaves);
    for (int i = 0; i < linhas; i++) {
        for (int j = 0; j < colunas; j++) {
            if (j > 0) fprintf(fp, " ");
            fprintf(fp, "%c", grade[i][j]);
        }
        fprintf(fp, "\n");
    }

    fclose(fp);

    for (int i = 0; i < linhas; i++) {
        free(grade[i]);
        free(visitado[i]);
        free(no_caminho[i]);
    }
    free(grade);
    free(visitado);
    free(no_caminho);
    free(caminho_lin);
    free(caminho_col);

    return 0;
}
