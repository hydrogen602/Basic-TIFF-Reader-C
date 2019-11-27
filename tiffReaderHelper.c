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

    uint32_t rowsPerStrip;
    get(rowsPerStrip, tag, 0);

    uint32_t stripsInImage = ceilDivision(t->height, rowsPerStrip);

    printf("rowsPerStrip  = %d\n", rowsPerStrip);
    printf("stripsInImage = %d\n", stripsInImage);

    tag = findTag(StripOffsets, t->tags, t->tagCount);
    assertNotNullPtrExc(tag);

    printf("stripOffsets  = ");
    uint32_t n;
    for (int i = 0; i < tag->dataCount; ++i) {
        get(n, tag, i);
        printf("%d, ", n);
    }
    putchar('\n');

    tag = findTag(PlanarConfiguration, t->tags, t->tagCount);
    assertNotNullPtrExc(tag);

    uint32_t planarConfig;
    get(planarConfig, tag, 0);

    printf("PlanarConfig  = %d\n", planarConfig);

    // i assume planar config of 1 is RGBRGBRGB while 2 is RRRGGGBBB

    tag = findTag(StripByteCounts, t->tags, t->tagCount);
    assertNotNullPtrExc(tag);

    printf("stripByteCounts = ");
    for (int i = 0; i < tag->dataCount; ++i) {
        get(n, tag, i);
        printf("%d, ", n);
    }
    putchar('\n');

    t->pixelsRed = malloc(t->height * t->width);
    t->pixelsGreen = malloc(t->height * t->width);
    t->pixelsBlue = malloc(t->height * t->width);

    

    return 0;
}