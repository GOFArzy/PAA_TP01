CC     = gcc
CFLAGS = -Wall -Wextra -std=c99 -Iinclude
 
# Alvo de teste do modulo mapa 
teste_mapa: testes/teste_mapa.c src/mapa.c include/mapa.h
	$(CC) $(CFLAGS) -o teste_mapa testes/teste_mapa.c src/mapa.c
 
clean:
	rm -f teste_mapa *.o
 
.PHONY: clean