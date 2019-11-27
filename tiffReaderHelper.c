#include "tiffReaderHelper.h"
#include "tiffTagStorage.h"

int ceilDivision(int a, int b) {
    // f = lambda total, rows: (total // rows) + (1 if total % rows > 0 else 0)

    return (a / b) + ((a % b > 0) ? 1 : 0);
}

int stripsReaderFunc(tiffImage_t* t, unsigned char* buffer, size_t fileSize) {

    tiffDataTag_t* tag = findTag(RowsPerStrip, t->tags, t->tagCount);

    if (tag == NULL) {
        printError("tag not found: RowsPerStrip");
    }

    uint32_t rowsPerStrip = tag->data[0]; // remember these are char pointers!!!! so this doesn't work

    uint32_t stripsInImage = ceilDivision(t->height, rowsPerStrip);

    printf("rowsPerStrip  = %d\n", rowsPerStrip);
    printf("stripsInImage = %d\n", stripsInImage);

    tag = findTag(StripOffsets, t->tags, t->tagCount);

    printf("stripOffsets  = ");
    uint32_t n;
    for (int i = 0; i < tag->dataCount; ++i) {
        get(n, tag, i);
        printf("%d, ", n);
    }
    putchar('\n');

    return 0;
}