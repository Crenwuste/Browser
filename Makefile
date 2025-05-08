# Traistaru Dragos-Andrei 313CA

EXEC = tema1

SRC = tema1.c

CC = gcc
CFLAGS = -Wall -Wextra -std=c99

build:
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)

run: build
	./$(EXEC)

pack:
	zip -FSr 313CA_TraistaruDragosAndrei_Tema1.zip README Makefile *.c *.h

clean:
	rm -f $(EXEC)

.PHONY: pack clean
