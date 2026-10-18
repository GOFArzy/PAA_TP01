#define _XOPEN_SOURCE 500
#define _DEFAULT_SOURCE

#include "visualizador.h"
#include <stdio.h>
#include <unistd.h>

void visualizador_limpar_tela(void) {
    printf("\033[H\033[J");
    fflush(stdout);
}

void visualizador_ocultar_cursor(void) {
    printf("\033[?25l");
    fflush(stdout);
}

void visualizador_mostrar_cursor(void) {
    printf("\033[?25h");
    fflush(stdout);
}

static const char *cor_para_celula(TipoCelula c) {
    switch (c) {
        case PAREDE:      return ANSI_BG_WHITE   ANSI_FG_BLACK;
        case SALA_VAZIA:  return ANSI_BG_BLACK   ANSI_FG_WHITE;
        case CHAVE:       return ANSI_BG_YELLOW  ANSI_FG_BLACK;
        case BAU:         return ANSI_BG_MAGENTA ANSI_FG_WHITE;
        case AVENTUREIRO: return ANSI_BG_GREEN   ANSI_FG_BLACK;
        default:          return ANSI_RESET;
    }
}

void visualizador_desenhar(const Labirinto *lab,
                           const Caminho *caminho,
                           int pos_atual) {
    if (!lab || !lab->grade) return;

    int **no_caminho = NULL;
    if (caminho) {
        no_caminho = alocar_matriz_int(lab->linhas, lab->colunas);
        if (no_caminho) {
            for (int k = 0; k < caminho->tamanho; k++) {
                int l = caminho->passos[k].linha;
                int c = caminho->passos[k].coluna;
                if (l >= 0 && l < lab->linhas && c >= 0 && c < lab->colunas)
                    no_caminho[l][c] = 1;
            }
        }
    }

    visualizador_limpar_tela();

    for (int i = 0; i < lab->linhas; i++) {
        for (int j = 0; j < lab->colunas; j++) {
            TipoCelula cel = lab->grade[i][j];
            const char *cor = cor_para_celula(cel);
            char ch = ' ';
            switch (cel) {
                case PAREDE:      ch = '#'; break;
                case SALA_VAZIA:  ch = '.'; break;
                case CHAVE:       ch = 'K'; break;
                case BAU:         ch = 'X'; break;
                case AVENTUREIRO: ch = 'A'; break;
            }

            if (no_caminho && no_caminho[i][j]) {
                if (pos_atual >= 0 && caminho && pos_atual < caminho->tamanho &&
                    caminho->passos[pos_atual].linha == i &&
                    caminho->passos[pos_atual].coluna == j)
                    printf(ANSI_BG_CYAN ANSI_FG_BLACK " @ " ANSI_RESET);
                else
                    printf(ANSI_BG_BLUE ANSI_FG_WHITE " * " ANSI_RESET);
            } else {
                printf("%s %c " ANSI_RESET, cor, ch);
            }
        }
        printf("\n");
    }

    if (no_caminho) liberar_matriz_int(no_caminho, lab->linhas);
    fflush(stdout);
}

void visualizador_desenhar_caminho_final(const Labirinto *lab,
                                         const Caminho *caminho) {
    if (!lab || !lab->grade) return;
    visualizador_desenhar(lab, caminho, -1);
}

void visualizador_animar_caminho(const Labirinto *lab,
                                 const Caminho *caminho,
                                 unsigned int delay_us) {
    if (!lab || !caminho || caminho->tamanho <= 0) return;

    visualizador_ocultar_cursor();

    for (int k = 0; k < caminho->tamanho; k++) {
        visualizador_desenhar(lab, caminho, k);
        fflush(stdout);
        usleep(delay_us);
    }

    visualizador_mostrar_cursor();
}