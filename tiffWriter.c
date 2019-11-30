#include "header.h"
#include "tiffTagStorage.h"
// todo

int ceilDivision(int a, int b) {
    // f = lambda total, rows: (total // rows) + (1 if total % rows > 0 else 0)

    return (a / b) + ((a % b > 0) ? 1 : 0);
}

int determineNumberOfStrips(tiffImage_t* t) {
    if (t->height == 0 || t->width == 0) {
        printErrMsg("Bad height or width of image");
        return -1;
    }


    tiffDataTag_t* test = findTag(RowsPerStrip, t->tags, t->tagCount);
    if (test != NULL & test->dataCount > 0) {
        // already known
        return 0;
    }

    uint64_t size = t->width * t->height;

    const int pixelsPerRow = 40000;

    int strips = size / pixelsPerRow;

    int rowsPerStrip = ceilDivision(t->height, strips);
    
    printf("Strips = %d\n", strips);
    printf("RowsPerStrip = %d\n", rowsPerStrip);

    tiffDataTag_t tag = newDataTag(RowsPerStrip, LONG_TypeID, 1);

    set(rowsPerStrip, &tag, 0);

    

    // add to tags

    return 0;
}

int _tiffWriter_openFile(const char* filename) {
    FILE* f = fopen(filename, "wb");

    if (f == NULL) {
        printErrMsg("Cannot open file");
    }




}