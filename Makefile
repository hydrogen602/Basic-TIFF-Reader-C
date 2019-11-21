CC = gcc
CFLAGS = -Wall -pedantic -O -std=c99
LDLIBS = -lm
# add $(LDLIBS) for math

# test

all: main.o
	$(CC) -o main main.o

clean:
	rm *.o main