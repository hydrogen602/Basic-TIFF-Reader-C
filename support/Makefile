CC = gcc
CFLAGS = -Wall -pedantic -O -std=c99
LDLIBS = -lm
VALGRIND = -O0 -g

array: array.c array.h ../header.h
	$(CC) $(VALGRIND) array.c -o array

testArray: array
	valgrind --leak-check=yes ./array