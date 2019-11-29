CC = gcc
CFLAGS = -Wall -pedantic -O -std=c99
LDLIBS = -lm
DEBUG = -D DEBUG
# add $(LDLIBS) for math
# add $(DEBUG) for debugging print statements

HEADERS := $(wildcard *.h)

all: main.o tiffTagStorage.o tiffReaderHelper.o tiffImage.o tiffReader.o
	$(CC) $(CFLAGS) -o main main.o tiffTagStorage.o tiffReaderHelper.o tiffImage.o tiffReader.o

main.o: main.c $(HEADERS)
	$(CC) $(CFLAGS) -c main.c -o main.o

tiffTagStorage.o: tiffTagStorage.c $(HEADERS)
	$(CC) $(CFLAGS) -c tiffTagStorage.c -o tiffTagStorage.o

tiffReaderHelper.o: tiffReaderHelper.c $(HEADERS)
	$(CC) $(CFLAGS) -c tiffReaderHelper.c -o tiffReaderHelper.o

tiffImage.o: tiffImage.c $(HEADERS)
	$(CC) $(CFLAGS) -c tiffImage.c -o tiffImage.o

tiffReader.o: tiffReader.c $(HEADERS)
	$(CC) $(CFLAGS) -c tiffReader.c -o tiffReader.o

clean:
	rm *.o main main