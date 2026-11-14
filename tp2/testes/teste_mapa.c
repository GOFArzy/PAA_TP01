#include <stdio.h>
#include "mapa.h"
 
int main(int argc, char *argv[])
{
    Mapa m;
    int i, j, r; 
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <arquivo_de_mapa>\n", argv[0]);
        return 2;
    } 
    r = mapa_ler(argv[1], &m);
    if (r != MAPA_OK) {
        fprintf(stderr, "mapa_ler falhou (codigo %d)\n", r);
        return 1;
    }
    printf("Dimensoes: %d x %d | HP inicial: %d\n", m.linhas, m.colunas, m.hp_inicial);
    printf("I = (%d, %d) | F = (%d, %d)\n",
           m.inicio_linha, m.inicio_coluna, m.fim_linha, m.fim_coluna);
    for (i = 0; i < m.linhas; i++) {
        for (j = 0; j < m.colunas; j++)
            printf("%4d", m.valor[i][j]);
        printf("\n");
    }
    mapa_liberar(&m);
    return 0;
}