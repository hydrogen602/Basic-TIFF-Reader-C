#include "tiffReaderHelper.h"
#include "tiffTagStorage.h"

int ceilDivision(int a, int b) {
    // f = lambda total, rows: (total // rows) + (1 if total % rows > 0 else 0)

    return (a / b) + ((a % b > 0) ? 1 : 0);
}

uint8_t _tiffReaderHelper_read1ByteFromBuffer(unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    if (offset + sizeof(uint8_t) - 1 >= fileSize || offset < 0) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException: %lu at line %d in file %s\n", offset + sizeof(uint8_t), __LINE__, __FILE__);
        exit(1);
    }

    uint8_t result = *(buffer + offset);
    // no byteswapping to do cause it's one byte

    return result;   
}

int stripsReaderFunc(tiffImage_t* t, unsigned char* buffer, size_t fileSize) {

    tiffDataTag_t* tag = findTag(RowsPerStrip, t->tags);

    if (tag == NULL) {
        printErrMsg("tag not found: RowsPerStrip");
    }

    uint32_t rowsPerStrip;
    /*
     * remember to initialize values set with memcpy as only part of them could be set and so garbage can be included in the number
     * initializing is happening inside of the get macro
     */
    get(rowsPerStrip, tag, 0);

    uint32_t stripsInImage = ceilDivision(t->height, rowsPerStrip);

    printf("rowsPerStrip  = %d\n", rowsPerStrip);
    printf("stripsInImage = %d\n", stripsInImage);

    tiffDataTag_t* stripOffsets_tag = findTag(StripOffsets, t->tags);
    assertNotNullPtrExc(stripOffsets_tag);

    printf("stripOffsets  = ");
    uint32_t n;
    for (int i = 0; i < stripOffsets_tag->dataCount; ++i) {
        get(n, stripOffsets_tag, i);
        printf("%d, ", n);
    }
    putchar('\n');

    tag = findTag(PlanarConfiguration, t->tags);
    assertNotNullPtrExc(tag);

    uint32_t planarConfig;
    get(planarConfig, tag, 0);

    printf("PlanarConfig  = %d\n", planarConfig);

    if (planarConfig != 1) {
        printErrMsg("Cannot yet handle this data arrangement");
    }

    // i assume planar config of 1 is RGBRGBRGB while 2 is RRRGGGBBB

    tiffDataTag_t* stripByteCounts_tag = findTag(StripByteCounts, t->tags);
    assertNotNullPtrExc(stripByteCounts_tag);

    printf("stripByteCounts = ");
    for (int i = 0; i < stripByteCounts_tag->dataCount; ++i) {
        get(n, stripByteCounts_tag, i);
        printf("%d, ", n);
    }
    putchar('\n');

    if (stripByteCounts_tag->dataCount != stripOffsets_tag->dataCount) {
        printErrMsg("StripByteCount and StripOffsetsCount are not equal");
        return -1;
    }

    t->pixelsRed = malloc(t->height * t->width * sizeof(pixel_t));
    t->pixelsGreen = malloc(t->height * t->width * sizeof(pixel_t));
    t->pixelsBlue = malloc(t->height * t->width * sizeof(pixel_t));

    if (t->pixelsRed == NULL || t->pixelsGreen == NULL || t->pixelsBlue == NULL) {
        printErrMsg("malloc failed");
        return EXIT_FAILURE;
    }

    printf("Allocated %ld bytes\n", t->height * t->width);

    uint32_t pixelIndex = 0;
    for (uint32_t i = 0; i < stripOffsets_tag->dataCount; ++i) {
        uint32_t sizeOfStrip;
        get(sizeOfStrip, stripByteCounts_tag, i);
        uint32_t offset;
        get(offset, stripOffsets_tag, i);
        printf("strip: len = %d, offset = %d\n", sizeOfStrip, offset);
        for (int32_t j = 0; j < 0; j = j + 3) {
            if (pixelIndex >= t->height * t->width) { printErrMsg("ArrayIndexOutOfBounds Exception"); return EXIT_FAILURE; }

            *(t->pixelsRed + pixelIndex) = _tiffReaderHelper_read1ByteFromBuffer(offset + j, buffer, fileSize);
            *(t->pixelsRed + pixelIndex) = _tiffReaderHelper_read1ByteFromBuffer(offset + j+1, buffer, fileSize);
            *(t->pixelsRed + pixelIndex) = _tiffReaderHelper_read1ByteFromBuffer(offset + j+2, buffer, fileSize);
            pixelIndex++;
        }
    }    

    return 0;
}