CC = gcc
CFLAGS = -Wall -pedantic -O -std=c99
LDLIBS = -lm
# add $(LDLIBS) for math

HEADERS := $(wildcard *.h)

all: main.o
	$(CC) -o main main.o

tiffReader: tiffReader.o tiffTagStorage.o tiffImage.o
	$(CC) -o tiffReader tiffReader.o tiffTagStorage.o tiffImage.o

tiffTagStorage.o: tiffTagStorage.c $(HEADERS)
	$(CC) $(CFLAGS) -c tiffTagStorage.c -o tiffTagStorage.o

tiffImage.o: tiffImage.c $(HEADERS)
	$(CC) $(CFLAGS) -c tiffImage.c -o tiffImage.o

tiffReader.o: tiffReader.c $(HEADERS)
	$(CC) $(CFLAGS) -c tiffReader.c -o tiffReader.o

clean:
	rm *.o main