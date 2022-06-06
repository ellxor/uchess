CC=clang
CFLAGS=-O3 -march=native

SRC=src/bits.c src/movegen.c src/position.c
OBJ=bits.o movegen.o position.o
LIB=libuchess.a

WARNINGS=-Wall -Wextra -pedantic -std=c99
IGNORE=-Wno-missing-field-initializers -Wno-gnu-binary-literal
WARNINGS+=$(IGNORE)

default: lib

unittest:
	$(CC) -o $@ $(SRC) src/unittest.c $(CFLAGS) $(WARNINGS)
	@echo
	./$@

lib:
	$(CC) -c $(SRC) $(CFLAGS) $(WARNINGS)
	ar rcs $(LIB) $(OBJ)
	rm -rf $(OBJ)
