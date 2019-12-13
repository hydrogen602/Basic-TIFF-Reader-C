CC = gcc
CFLAGS = -Wall -pedantic -O -std=c99
LDLIBS = -lm
VALGRIND = -O0 -g -D TESTING

HEADERS := $(wildcard *.h)

array: array.c array.h ../header.h
	$(CC) $(VALGRIND) array.c -o array

.PHONY: testArray
testArray: array
	valgrind --leak-check=yes ./array

