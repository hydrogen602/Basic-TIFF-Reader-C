CC = gcc
CFLAGS = -Wall -pedantic -O -std=c11
LDLIBS = -lm
DEBUG = -D DEBUG
VALGRIND = -O0 -g
DEBUGGER = -g -O0
# add $(LDLIBS) for math
# add $(DEBUG) for debugging print statements

HEADERS := $(wildcard *.h)
HEADERS += $(wildcard support/*.h)

all: main.o tiffTagStorage.o tiffReaderHelper.o tiffImage.o tiffReader.o array.o
	$(CC) $(CFLAGS) -o main main.o tiffTagStorage.o tiffReaderHelper.o tiffImage.o tiffReader.o array.o

tiffWriter: tiffWriter.o tiffTagStorage.o tiffImage.o array.o
	$(CC) $(CFLAGS) -o tiffWriter tiffWriter.o tiffTagStorage.o tiffImage.o array.o

#main.o: main.c $(HEADERS)
#	$(CC) $(CFLAGS) -c main.c -o main.o

#tiffTagStorage.o: tiffTagStorage.c $(HEADERS)
#	$(CC) $(CFLAGS) -c tiffTagStorage.c -o tiffTagStorage.o

#tiffReaderHelper.o: tiffReaderHelper.c $(HEADERS)
#	$(CC) $(CFLAGS) -c tiffReaderHelper.c -o tiffReaderHelper.o

#tiffImage.o: tiffImage.c $(HEADERS)
#	$(CC) $(CFLAGS) -c tiffImage.c -o tiffImage.o

#tiffReader.o: tiffReader.c $(HEADERS)
#	$(CC) $(CFLAGS) -c tiffReader.c -o tiffReader.o

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $*.c -o $@

# support code
array.o: support/array.c $(HEADERS)
	$(CC) $(CFLAGS) -c support/array.c -o array.o

.PHONY: writerDebug
writerDebug: CFLAGS += $(DEBUGGER)
writerDebug: clean tiffWriter
	@echo "done compiling"


.PHONY: mainDebug
mainDebug: CFLAGS += $(DEBUGGER)
mainDebug: clean all
	@echo "done compiling"

.PHONY: clean
clean:
	rm -f *.o main tiffWriter