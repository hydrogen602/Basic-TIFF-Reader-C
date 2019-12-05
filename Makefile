CC = gcc
CFLAGS = -Wall -pedantic -O -std=c99
LDLIBS = -lm
DEBUG = -D DEBUG
VALGRIND = -O0 -g
# add $(LDLIBS) for math
# add $(DEBUG) for debugging print statements

HEADERS := $(wildcard *.h)
HEADERS += $(wildcard support/*.h)

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

# support code
array.o: support/array.c $(HEADERS)
	$(CC) $(CFLAGS) -c support/array.c -o array.o

.PHONY: clean
clean:
	rm *.o main