#ifndef MAPA_H
#define MAPA_H

/* Tipos de celula do labirinto */
typedef enum {
    SALA_VAZIA  = 0,  /* '0' - sala livre para passagem */
    PAREDE      = 1,  /* '1' - obstaculo intransponivel */
    CHAVE       = 2,  /* 'C' - sala contendo chave */
    BAU         = 3,  /* 'X' - sala com o bau do tesouro */
    AVENTUREIRO = 4   /* 'A' - posicao inicial */
} TipoCelula;

/* Estrutura principal do labirinto */
typedef struct {
    int linhas;
    int colunas;
    int chaves_necessarias;
    TipoCelula **grade;
    int inicio_lin;
    int inicio_col;
    int bau_lin;
    int bau_col;
} Labirinto;

/* Coordenada bidimensional (linha, coluna) */
typedef struct {
    int linha;
    int coluna;
} Posicao;

/* Array dinamico para armazenamento de sequencias de passos */
typedef struct {
    Posicao *passos;
    int tamanho;
    int capacidade;
} Caminho;

/* Leitura e liberacao da matriz do labirinto */
Labirinto* carregar_labirinto(const char *arquivo);
void liberar_labirinto(Labirinto *lab);

/* Alocacao e liberacao de matriz auxiliar para visitados */
int** alocar_matriz_int(int linhas, int colunas);
void liberar_matriz_int(int **mat, int linhas);

/* Manipulacao do array dinamico de posicoes */
void caminho_init(Caminho *c);
void caminho_adicionar(Caminho *c, int lin, int col);
void caminho_remover_ultimo(Caminho *c);
Caminho caminho_copiar(const Caminho *c);
void caminho_liberar(Caminho *c);

#endif /* MAPA_H */
