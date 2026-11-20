CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Iinclude
LDFLAGS = 

# Executaveis e alvos principais
EXEC_PRINCIPAL = tp2
EXEC_GERADOR   = gerador
TEST_MAPA      = teste_mapa
TEST_CAMINHO   = teste_caminho
TEST_SAIDA     = teste_saida

# Objetos do programa principal
OBJS_PRINCIPAL = src/main.o src/mapa.o src/dp.o src/caminho.o src/saida.o

# Objetos dos programas de teste e ferramentas
OBJ_GERADOR      = src/gerador.o
OBJ_TEST_MAPA    = testes/teste_mapa.o src/mapa.o
OBJ_TEST_CAMINHO = testes/teste_caminho.o src/caminho.o src/dp.o src/mapa.o
OBJ_TEST_SAIDA   = testes/teste_saida.o src/saida.o

# Alvo padrao: compila o executavel principal e todas as ferramentas/testes
all: $(EXEC_PRINCIPAL) $(EXEC_GERADOR) $(TEST_MAPA) $(TEST_CAMINHO) $(TEST_SAIDA)

# Programa principal (Gabs)
$(EXEC_PRINCIPAL): $(OBJS_PRINCIPAL)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Ferramenta independente geradora de mapas (Extra 4 - Gabs)
$(EXEC_GERADOR): $(OBJ_GERADOR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Testes unitarios isolados dos modulos
$(TEST_MAPA): $(OBJ_TEST_MAPA)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_CAMINHO): $(OBJ_TEST_CAMINHO)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_SAIDA): $(OBJ_TEST_SAIDA)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Regra generica de compilacao de objetos .o a partir de .c
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Executa bateria completa de testes automatizados
test: all
ifeq ($(OS),Windows_NT)
	@echo [1/4] Executando testes do modulo mapa...
	@.\$(TEST_MAPA).exe testes/Caverna1.txt
	@echo [2/4] Executando testes do modulo caminho...
	@.\$(TEST_CAMINHO).exe
	@echo [3/4] Executando testes do modulo saida...
	@.\$(TEST_SAIDA).exe
	@echo [4/4] Executando teste integrado do executavel principal...
	@.\$(EXEC_PRINCIPAL).exe testes/Caverna1.txt resultado.txt
else
	@echo "[1/4] Executando testes do modulo mapa..."
	@./$(TEST_MAPA) testes/Caverna1.txt
	@echo "[2/4] Executando testes do modulo caminho..."
	@./$(TEST_CAMINHO)
	@echo "[3/4] Executando testes do modulo saida..."
	@./$(TEST_SAIDA)
	@echo "[4/4] Executando teste integrado do executavel principal..."
	@./$(EXEC_PRINCIPAL) testes/Caverna1.txt resultado.txt
endif

# Execucao rapida do programa principal com o caso base
run: $(EXEC_PRINCIPAL)
ifeq ($(OS),Windows_NT)
	.\$(EXEC_PRINCIPAL).exe testes/Caverna1.txt
else
	./$(EXEC_PRINCIPAL) testes/Caverna1.txt
endif

# Limpeza de binarios e arquivos intermediarios
clean:
ifeq ($(OS),Windows_NT)
	-cmd /c del /f /q $(EXEC_PRINCIPAL).exe $(EXEC_GERADOR).exe $(TEST_MAPA).exe $(TEST_CAMINHO).exe $(TEST_SAIDA).exe src\*.o testes\*.o *.o resultado.txt 2>nul
else
	rm -f $(EXEC_PRINCIPAL) $(EXEC_GERADOR) $(TEST_MAPA) $(TEST_CAMINHO) $(TEST_SAIDA) src/*.o testes/*.o *.o resultado.txt
endif

.PHONY: all test run clean
