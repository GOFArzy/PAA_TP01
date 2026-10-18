# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I./include -g

# Diretórios
SRC_DIR = src
INC_DIR = include
OBJ_DIR = obj
BIN_DIR = bin

# Compatibilidade de comandos entre Windows e Linux
ifeq ($(OS),Windows_NT)
    MKDIR = if not exist $(OBJ_DIR) mkdir $(OBJ_DIR) & if not exist $(BIN_DIR) mkdir $(BIN_DIR)
    RMDIR = if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR) & if exist $(BIN_DIR) rmdir /s /q $(BIN_DIR)
else
    MKDIR = mkdir -p $(OBJ_DIR) $(BIN_DIR)
    RMDIR = rm -rf $(OBJ_DIR) $(BIN_DIR)
endif

# Arquivos fonte do programa principal (labirinto)
MAIN_SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/mapa.c $(SRC_DIR)/backtracking.c \
            $(SRC_DIR)/navegacao.c $(SRC_DIR)/cli.c $(SRC_DIR)/analise.c \
            $(SRC_DIR)/visualizador.c $(SRC_DIR)/saida.c
MAIN_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(MAIN_SRCS))

# Arquivos fonte do gerador (Extra 1)
GERADOR_SRCS = $(SRC_DIR)/gerador.c
GERADOR_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(GERADOR_SRCS))

# Arquivos fonte do benchmark (Extra 5)
BENCH_SRCS = $(SRC_DIR)/benchmark.c $(SRC_DIR)/mapa.c $(SRC_DIR)/backtracking.c \
             $(SRC_DIR)/navegacao.c $(SRC_DIR)/analise.c $(SRC_DIR)/saida.c
BENCH_OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(BENCH_SRCS))

# Executáveis
TARGET_MAIN = $(BIN_DIR)/labirinto
TARGET_GERADOR = $(BIN_DIR)/gerador
TARGET_BENCHMARK = $(BIN_DIR)/benchmark

# Regra padrão
all: dirs $(TARGET_MAIN) $(TARGET_GERADOR) $(TARGET_BENCHMARK)

# Criar diretórios necessários
dirs:
	@$(MKDIR)

# Compilar o Labirinto Principal
$(TARGET_MAIN): $(MAIN_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compilar o Gerador de Mapas
$(TARGET_GERADOR): $(GERADOR_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# Compilar o Benchmark
$(TARGET_BENCHMARK): $(BENCH_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lm

# Regra genérica para gerar arquivos objeto (.o)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza dos arquivos compilados
clean:
	@$(RMDIR)

.PHONY: all dirs clean
