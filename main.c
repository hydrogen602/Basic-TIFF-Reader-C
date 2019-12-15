#include "header.h"
#include "tiffReader.h"

int main(void) {
    const char* c = "test.tif";

    tiffFile_t t;
    readFile(c, &t);

    freeFile(&t);

    printf("Done\n");
}