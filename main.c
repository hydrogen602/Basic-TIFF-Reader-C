#include "header.h"
#include "tiffReader.h"

int main(void) {
    const char* c = "test100x100.tiff";

    tiffFile_t t;
    readFile(c, &t);

    freeFile(&t);

    printf("Done\n");
}