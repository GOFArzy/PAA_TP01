CC     = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude

all: teste_mapa teste_caminho

# Alvo de teste do modulo mapa (Rafa)
teste_mapa: testes/teste_mapa.c src/mapa.c include/mapa.h
	$(CC) $(CFLAGS) -o teste_mapa testes/teste_mapa.c src/mapa.c

# Alvo de teste do modulo caminho (Otavio)
teste_caminho: testes/teste_caminho.c src/caminho.c src/dp.c src/mapa.c include/caminho.h include/dp.h include/mapa.h
	$(CC) $(CFLAGS) -o teste_caminho testes/teste_caminho.c src/caminho.c src/dp.c src/mapa.c

clean:
ifeq ($(OS),Windows_NT)
	-cmd /c del /f /q teste_mapa.exe teste_caminho.exe *.o 2>nul
else
	rm -f teste_mapa teste_caminho *.o
endif

.PHONY: all clean
