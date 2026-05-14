# Makefile — Simulador de Sessão de Recarga

CC      = gcc
CFLAGS  = -Wall -Wextra -pedantic -std=c99
TARGET  = recarga

.PHONY: all clean

all: $(TARGET)

$(TARGET): recarga.c
	$(CC) $(CFLAGS) -o $(TARGET) recarga.c

clean:
	rm -f $(TARGET)
