#include "header.h"
#include "tiffTagStorage.h"
#include "tiffImage.h"
#include "support/array.h"

#define fwriteValue(value, stream) { byte * ptr = (byte*)(&(value)); for (int i = 0; i < sizeof(value); ++i) { fputc(*(ptr + i), (stream)); } }
//  fwrite(&(value), sizeof(value), 1, (stream))//; printf("%x ", value)

int ceilDivision(int a, int b) {
    // f = lambda total, rows: (total // rows) + (1 if total % rows > 0 else 0)

    return (a / b) + ((a % b > 0) ? 1 : 0);
}

int __createWitdhHeightTag(tiffImage_t* t) {
    tiffDataTag_t* hTag = findTag(ImageHeight, t->tags);
    if (hTag != NULL) {
        // nothing to do
    }
    else {
        tiffDataTag_t tag = newDataTag(ImageHeight, LONG_TypeID, 1);
        set(t->height, &tag, 0);

        // add to tags
        append(t->tags, tag);
    }

    tiffDataTag_t* wTag = findTag(ImageWidth, t->tags);
    if (wTag != NULL) {
        // nothing to do
    }
    else {
        tiffDataTag_t tag = newDataTag(ImageWidth, LONG_TypeID, 1);
        set(t->width, &tag, 0);

        // add to tags
        append(t->tags, tag);
    }

    return 0;
}

int __createRowsPerStripTag(tiffImage_t* t) {
    tiffDataTag_t* test = findTag(RowsPerStrip, t->tags);
    tiffDataTag_t* test2 = findTag(StripByteCounts, t->tags);

    if (test != NULL && test2 != NULL) {
        // already known
        return 0;
    }
    else if (test == NULL && test2 == NULL) {
        // make tags
    }
    else {
        // only one of two exists, issue!
        printErrMsg("Only one of RowsPerStrip and StripByteCounts exists, should either both exist or neither");
        return -1;
    }

    uint64_t size = t->width * t->height;

    const int pixelsPerRow = 40000;

    int strips = size / pixelsPerRow;
    if (strips == 0) {
        strips = 1;
    }

    int rowsPerStrip = ceilDivision(t->height, strips);
    
    printf("Strips = %d\n", strips);
    printf("RowsPerStrip = %d\n", rowsPerStrip);

    tiffDataTag_t tag = newDataTag(RowsPerStrip, LONG_TypeID, 1);

    set(rowsPerStrip, &tag, 0);

    // add to tags
    append(t->tags, tag);

    tiffDataTag_t tag2 = newDataTag(StripByteCounts, LONG_TypeID, 1);

    uint64_t byteCountPerStrip = rowsPerStrip * t->width;
    if (t->type == RGB) {
        byteCountPerStrip = byteCountPerStrip * 3;
        size = size * 3;
    }

    //uint64_t byteCounts[strips];

    for (int i = 0; i < strips - 1; ++i) {
    //    byteCounts[i] = byteCountPerStrip;
        set(byteCountPerStrip, &tag2, i);
    }
    // last one shorter
    //byteCounts[strips - 1] = (size % byteCountPerStrip == 0) ? byteCountPerStrip : (size % byteCountPerStrip);
    uint64_t n = (size % byteCountPerStrip == 0) ? byteCountPerStrip : (size % byteCountPerStrip);
    set(n, &tag2, strips - 1);

    //for (int i = 0; i < strips; ++i) {
    //    set(byteCounts[i], &tag2, i);
    //    printf("byteCounts[%d] = %lu\n", i, byteCounts[i]);
    //}

    append(t->tags, tag2);

    tiffDataTag_t newTag = newDataTag(StripOffsets, LONG_TypeID, strips);

    for (int i = 0; i < strips; ++i) {
        uint64_t n = TIFF_HEADER_LENGTH + byteCountPerStrip * i;
        set(n, &newTag, i);
    }

    tiffDataTag_t *tag_ptr = findTag(StripOffsets, t->tags);

    if (tag_ptr == NULL) {
        append(t->tags, newTag);
    }
    else {
        tag_ptr->dataType = LONG_TypeID;
        tag_ptr->dataCount = strips;
        if (tag_ptr->data != NULL) {
            free(tag_ptr->data);
        }
        tag_ptr->data = newTag.data;
    }

    return 0;
}

int __createMiscTags(tiffImage_t* t) {
    tiffDataTag_t *test = findTag(XResolution, t->tags);
    if (test != NULL) {
        // nothing to do
    }
    else {
        tiffDataTag_t tag = newDataTag(XResolution, RATIONAL_TypeID, 1);
        RATIONAL rat = { 3 / 1 };
        
        DWORD * ptr = (DWORD*) tag.data;
        ptr[0] = rat.num;
        ptr[1] = rat.denom;

        append(t->tags, tag);
    }

    test = findTag(YResolution, t->tags);
    if (test != NULL) {
        // nothing to do
    }
    else {
        tiffDataTag_t tag = newDataTag(YResolution, RATIONAL_TypeID, 1);
        RATIONAL rat = { 3 / 1 };
        
        DWORD * ptr = (DWORD*) tag.data;
        ptr[0] = rat.num;
        ptr[1] = rat.denom;

        append(t->tags, tag);
    }

    test = findTag(ResolutionUnit, t->tags);
    if (test != NULL) {
        // nothing to do
    }
    else {
        tiffDataTag_t tag = newDataTag(ResolutionUnit, SHORT_TypeID, 1);
        SHORT n = 3; // 1 = none, 2 = inch, 3 = cm

        SHORT * ptr = (SHORT*) tag.data;
        ptr[0] = n;

        append(t->tags, tag);
    }

    test = findTag(NewSubFileType, t->tags);
    if (test != NULL) {
        // nothing to do
    }
    else {
        tiffDataTag_t tag = newDataTag(NewSubFileType, LONG_TypeID, 1);
        LONG n = 0;
        set(n, &tag, 0);

        append(t->tags, tag);
    }

    if (t->type == RGB) {
        test = findTag(PlanarConfiguration, t->tags);
        if (test != NULL) {
            // nothing to do
        }
        else {
            tiffDataTag_t tag = newDataTag(PlanarConfiguration, SHORT_TypeID, 1);
            int n = 1;
            set(n, &tag, 0);

            append(t->tags, tag);
        }
    }

    test = findTag(SamplesPerPixel, t->tags);
    if (test != NULL) {
        // nothing to do
    }
    else {
        tiffDataTag_t tag = newDataTag(SamplesPerPixel, SHORT_TypeID, 1);
        SHORT n = 0;
        if (t->type == RGB) {
            n = 3;
        }
        else if (t->type == GRAY_SCALE) {
            n = 1;
        }
        else {
            printErrMsg("Invalid Image type, expected RGB or GRAY_SCALE");
            return -1;
        }
        set(n, &tag, 0);

        append(t->tags, tag);
    }

    test = findTag(BitsPerSample, t->tags);
    if (test != NULL) {
        // nothing to do
    }
    else {
        
        tiffDataTag_t tag;
        if (t->type == RGB) {
            tag = newDataTag(BitsPerSample, SHORT_TypeID, 3);
            int n = 8;
            set(n, &tag, 0);
            set(n, &tag, 1);
            set(n, &tag, 2);
        }
        else if (t->type == GRAY_SCALE) {
            tag = newDataTag(BitsPerSample, SHORT_TypeID, 1);
            int n = 8;
            set(n, &tag, 0);
        }
        else {
            printErrMsg("Invalid Image type, expected RGB or GRAY_SCALE");
            return -1;
        }

        // Compression

        append(t->tags, tag);
    }

    test = findTag(Compression, t->tags);
    if (test != NULL) {
        // nothing to do
    }
    else {
        tiffDataTag_t tag = newDataTag(Compression, SHORT_TypeID, 1);
        SHORT n = C_Uncompressed;
        set(n, &tag, 0);

        append(t->tags, tag);
    }

    test = findTag(PhotometricInterpretation, t->tags);
    if (test != NULL) {
        // nothing to do
    }
    else {
        tiffDataTag_t tag = newDataTag(PhotometricInterpretation, SHORT_TypeID, 1);
        SHORT n;
        if (t->type == RGB) {
            n = PI_RGB;
        }
        else if (t->type == GRAY_SCALE) {
            n = PI_BlackIsZero;
        }
        else {
            printErrMsg("Invalid Image type, expected RGB or GRAY_SCALE");
            return -1;
        }
        set(n, &tag, 0);

        append(t->tags, tag);
    }

    return 0;
}

int createMissingTags(tiffImage_t* t) {
    if (t->height == 0 || t->width == 0) {
        printErrMsg("Bad height or width of image");
        return -1;
    }

    int r;
    r = __createWitdhHeightTag(t);
    if (r != 0) { return r; }
    r = __createRowsPerStripTag(t);
    if (r != 0) { return r; }
    r = __createMiscTags(t);
    if (r != 0) { return r; }

    return 0;
}

int _tiffWriter_openFileAndWrite(const char* filename, unsigned char* buffer) {
    FILE* f = fopen(filename, "wb");

    if (f == NULL) {
        printErrMsg("Cannot open file");
    }

    for (size_t i = 0; i < len(buffer); ++i) {
        if (fputc(buffer[i], f) == EOF) {
            printErrMsg("write failed");
            return -1;
        }
    }

    fclose(f);

    return 0;
}

int imageWriter(tiffFile_t tf, const char * filename) {
    if (tf.imagesCount != 1) {
        printErrMsg("Unsupported. Must have one and only one image in file");
        return -1;
    }

    if (tf.byteOrder != TIFF_BIG_ENDIAN && tf.byteOrder != TIFF_LITTLE_ENDIAN) {
        printErrMsg("Invalid byte order code");
        return -1;
    }

    FILE * f = fopen(filename, "wb");
    if (f == NULL) {
        printErrMsg("File opening failed");
        return -1;
    }

    // write header

    uint16_t endianess = TIFF_LITTLE_ENDIAN;

    fwriteValue(endianess, f); // cause i'm on a little endian machine

    uint16_t version = TIFF_VERSION_BYTEFLIP;

    fwriteValue(version, f);

    tiffImage_t img = tf.images[0];

    uint32_t offsetIFD = TIFF_HEADER_LENGTH + ((img.type == RGB) ? 3 * img.height * img.width : img.height * img.width);

    fwriteValue(offsetIFD, f);

    printf("offsetIFD = 0x%x\n", offsetIFD);

    // write image data

    if (img.type == RGB) {
        tiffDataTag_t* tag = findTag(PlanarConfiguration, img.tags);

        if (tag == NULL) {
            printErrMsg("Missing Tag PlanarConfiguration");
            fclose(f);
            return -1;
        }

        unsigned short n;
        get(n, tag, 0);
        if (n != 1) {
            printErrMsg("Planes not supported");
            fclose(f);
            return -1;
        }

        for (size_t i = 0; i < img.height * img.width; ++i) {
            fputc(img.pixelsRed[i], f);
            fputc(img.pixelsGreen[i], f);
            fputc(img.pixelsBlue[i], f);
        }
    }
    else if (img.type == GRAY_SCALE) {
        for (size_t i = 0; i < img.height * img.width; ++i) {
            fputc(img.pixels[i], f);
        }
    }
    else {
        printErrMsg("Unknown type");
        return -1;
    }

    // tags

    uint16_t s = len(img.tags);
    fwriteValue(s, f);

    //byte tagBuffer[12];

    size_t locationAfter = len(img.tags) * 12UL + TIFF_HEADER_LENGTH + 2UL + 4UL; // 2UL for tag count, 4UL for next IFD offset
    locationAfter += (img.type == RGB) ? 3 * img.height * img.width : img.height * img.width;

    byte* extraData = newBuffer(0);

    for (int i = 0; i < len(img.tags); ++i) {
        tiffDataTag_t tag = img.tags[i];

        tagPrintDebug(&tag);

        if (tag.tagId == 0x11b) {
            // to do
            printf("hi\n");
        }

        fwriteValue(tag.tagId, f);
        fwriteValue(tag.dataType, f);
        fwriteValue(tag.dataCount, f);

        //memcpy(tagBuffer, &(tag.tagId), sizeof(tag.tagId));
        //memcpy(tagBuffer + sizeof(WORD), &(tag.dataType), sizeof(tag.dataType));
        //memcpy(tagBuffer + 2*sizeof(WORD), &(tag.dataCount), sizeof(tag.dataCount));

        DWORD dataOffset = 0;
        if (getTypeSizeOf(tag.dataType) <= 4 && tag.dataCount == 1) {
            dataOffset = *(tag.data);
        }
        else {
            size_t currLen = len(extraData);
            dataOffset = locationAfter + currLen;
            extraData = checkBufferIndex(extraData, currLen, getTypeSizeOf(tag.dataType) * tag.dataCount);
            memcpy(extraData + currLen, tag.data, getTypeSizeOf(tag.dataType) * tag.dataCount);
        }
        fwriteValue(dataOffset, f);



        //memcpy(tagBuffer + 2*sizeof(WORD) + sizeof(DWORD), &dataOffset, sizeof(dataOffset));

        //fwrite(tagBuffer, sizeof(tagBuffer), 1, f);


    }

    uint32_t n = 0;

    fwrite(&n, sizeof(uint32_t), 1, f); // next IFD offset

    fwrite(extraData, len(extraData), 1, f);


    fclose(f);
    return 0;
}

int main(void) {
    /*
     * Outline of tiff structure I'm going to use
     * 
     * Tiff Header
     * Image data
     * IFD
     * other data
     */
    tiffImage_t img = newImage(RGB);
    img.height = 100;
    img.width = 100;

    img.pixelsBlue = newArray(100 * 100, 1);
    img.pixelsGreen = newArray(100 * 100, 1);
    img.pixelsRed = newArray(100 * 100, 1);

    for (int i = 1; i < 10000; ++i) {
        img.pixelsRed[i] = 0xff;
        img.pixelsGreen[i] = 0xff;
        img.pixelsBlue[i] = 0xff;
    }

    int r = createMissingTags(&img);
    if (r != 0) {
        printErrMsg("createMissingTags failed");
    }

    if (!isValidImage(&img)) {
        printErrMsg("Not valid image, cannot write to file");
        freeImage(&img);
        return -1;
    }

    tiffFile_t tifFile = newFile(&img, 1);

    imageWriter(tifFile, "test.tif");

    freeImage(&img);

}
