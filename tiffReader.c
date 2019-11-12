#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../header.h"
#include "tiffReader.h"

#define _printData printf(": %d\n", (t->TagList + i)->DataOffset)
#define _printDataType printf(": %d\n", (t->TagList + i)->DataType)
#define _printDataCount printf(": %d\n", (t->TagList + i)->DataCount)

boolean isProperHeader(TIFHEAD* t) {
    if (t->identifier != 0x4d4d && t->identifier != 0x4949) {
        return false;
    }
    if (t->version != 0x002a) {
        return false;
    }
    return true;
}

int sizeofFile(const char* filename) {
	// returns bytes in file or -1 upon failure
	// prints error msg

    FILE* f = fopen(filename, "rb");
    assertNotNullEx(f, "Cannot open file %s\n", filename);

    int count = 0;
    while (fgetc(f) != EOF)
    {
        count += 1;
    }

    fclose(f);

    return count;
}

int fileReader(const char* filename, unsigned char* buffer, unsigned int fileSize) {
    FILE *f = fopen(filename, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "Cannot open file %s\n", filename);
        return -1;
    }

    int ch;
    int count = 0;
    while ((ch = fgetc(f)) != EOF)
    {
        assertCmd(count < fileSize, "File grew in size during program execution\n", fclose(f));
		
		*(buffer + count) = (unsigned char)(ch);
        count += 1;
    }

    assertCmd(count == fileSize, "File shrunk in size during program execution\n", fclose(f));

    fclose(f);

    return 0;
}

short readShortFromBuffer(boolean littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    if (offset + sizeof(short) >= fileSize) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException: %lu\n", offset + sizeof(short));
        exit(1);
    }

    short result;
    if (littleEndian) {
        memcpy(&result, buffer + offset, sizeof(short));
    }
    else {
        short tmp;
        memcpy(&tmp, buffer + offset, sizeof(short));
        result = __builtin_bswap16(tmp);        
    }

    return result;
}

int readIntFromBuffer(boolean littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    if (offset + sizeof(int) >= fileSize) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException: %lu\n", offset + sizeof(int));
        exit(1);
    }

    int result;
    if (littleEndian) {
        memcpy(&result, buffer + offset, sizeof(int));
    }
    else {
        int tmp;
        memcpy(&tmp, buffer + offset, sizeof(int));
        result = __builtin_bswap32(tmp);        
    }

    return result;
}

int parseHeader(TIFHEAD* t, unsigned char* buffer, unsigned int fileSize) {
    // will probably only compile with gcc

    // basic tiff header is 8 bytes long
    assert(fileSize >= 8, "Image doesn't even have a header\n");

    unsigned int positionInMemory = 0;

    memcpy(&(t->identifier), buffer, sizeof(t->identifier));
    positionInMemory += sizeof(t->identifier);

    /* 
     * runs with the assumption of being run on a little-endian machine 
     * cause I don't want to kill myself over byte order 
     */

    boolean littleEndian = -1;
    if (t->identifier == 0x4d4d) {
        littleEndian = false; // big endian
    }
    else if (t->identifier == 0x4949) {
        littleEndian = true;
    }
    else {
        fprintf(stderr, "This is not a tiff file\n");
        return -1;
    }

    t->version = readShortFromBuffer(littleEndian, positionInMemory, buffer, fileSize);
    positionInMemory += sizeof(short);

    t->IFDOffset = readIntFromBuffer(littleEndian, positionInMemory, buffer, fileSize);
    positionInMemory += sizeof(int);
    
    printf("got number: %x\n", t->identifier);
    printf("got version: %x\n", t->version);
    printf("got offset: %x\n", t->IFDOffset);

    assert(isProperHeader(t), "Header incorrect\n");

    parseIFD(&(t->nextIDF), littleEndian, t->IFDOffset, buffer, fileSize);
    free(t->nextIDF.TagList);
    free(t->nextIDF.bitsPerSample);
    free(t->nextIDF.stripByteCounts);

    return 0;
}

int parseIFD(TIFIFD* t, boolean littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    // offset + buffer should be where the IFD is
    t->NumDirEntries = readShortFromBuffer(littleEndian, offset, buffer, fileSize);
    offset += sizeof(short);

    printf("Number of tags: %d\n", t->NumDirEntries);

    // remember to free this memory, len = t.NumDirEntries
    t->TagList = malloc(t->NumDirEntries * sizeof(TIFTAG));
    assertNotNull(t->TagList, "Failed to allocate memory\n");

    printf("Offset: %d\n", offset);

    for (int i = 0; i < t->NumDirEntries; i++) {
        (t->TagList + i)->TagId = readShortFromBuffer(littleEndian, offset, buffer, fileSize);
        offset += sizeof(short);
        printf("TagId: %d\n", (t->TagList + i)->TagId);

        (t->TagList + i)->DataType = readShortFromBuffer(littleEndian, offset, buffer, fileSize);
        offset += sizeof(short);

        (t->TagList + i)->DataCount = readIntFromBuffer(littleEndian, offset, buffer, fileSize);
        offset += sizeof(int);

        if ((t->TagList + i)->DataType == 3 && (t->TagList + i)->DataCount == 1) {

            (t->TagList + i)->DataOffset = readShortFromBuffer(littleEndian, offset, buffer, fileSize);
        }
        else {
            (t->TagList + i)->DataOffset = readIntFromBuffer(littleEndian, offset, buffer, fileSize);
        }

        offset += sizeof(int);

        unsigned int tmpOffset = 0;
        switch ((t->TagList + i)->TagId)
        {
        case 256:
            assert((t->TagList + i)->DataType == 3 || (t->TagList + i)->DataType == 4, "width data type wrong\n");

            t->width = (t->TagList + i)->DataOffset;
            printf("ImageWidth"); _printData;
            break;
        case 257:
            assert((t->TagList + i)->DataType == 3 || (t->TagList + i)->DataType == 4, "height data type wrong\n");
            
            t->height = (t->TagList + i)->DataOffset;
            printf("ImageHeight"); _printData;
            break;
        case 258:
            assert((t->TagList + i)->DataType == 3, "BitsPerSample data type wrong\n");

            tmpOffset = (t->TagList + i)->DataOffset;
            t->bitsPerSample = malloc(sizeof(short) * (t->TagList + i)->DataCount);
            assertNotNull(t->bitsPerSample, "malloc at bitsPerSample failed")
            for (int index = 0; index < (t->TagList + i)->DataCount; index++) {
                *(t->bitsPerSample + index) = readShortFromBuffer(littleEndian, tmpOffset, buffer, fileSize);
                printf("BitsPerSample: %d\n", *(t->bitsPerSample + index));
                tmpOffset += sizeof(short);
            }
            break;
        case 259:
            assert((t->TagList + i)->DataType == 3, "Compression data type wrong\n");

            printf("Compression"); _printData;

            assert((t->TagList + i)->DataOffset == 1, "This program can't deal with compression\n");
            t->compression = (t->TagList + i)->DataOffset;
            break;
        case 262:
            // what is this?
            assert((t->TagList + i)->DataType == 3, "PhotometricInterpretation data type wrong\n");
            printf("PhotometricInterpretation"); _printData;
            t->photometricInterpretation = (t->TagList + i)->DataOffset;
            break;
        case 273:
            assert((t->TagList + i)->DataType == 3 || (t->TagList + i)->DataType == 4, "StripOffsets data type wrong\n");
            
            t->stripCount = (t->TagList + i)->DataCount;
            t->stripOffsets = malloc((t->TagList + i)->DataCount * sizeof(DWORD));
            assertNotNull(t->stripOffsets, "malloc at stripOffsets failed\n");
            
            tmpOffset = (t->TagList + i)->DataOffset;
            for (int index = 0; index < (t->TagList + i)->DataCount; index++) {
                if ((t->TagList + i)->DataType == 3){
                    *(t->stripOffsets + index) = readShortFromBuffer(littleEndian, tmpOffset, buffer, fileSize);
                    tmpOffset += sizeof(short);
                }
                else {
                    *(t->stripOffsets + index) = readIntFromBuffer(littleEndian, tmpOffset, buffer, fileSize);
                    tmpOffset += sizeof(int);
                }
                printf("StripOffsets: %d\n", *(t->stripOffsets + index));
            }
            break;
        case 277:
            assert((t->TagList + i)->DataType == 3, "SamplesPerPixel data type wrong\n");
            
            t->samplesPerPixel = (t->TagList + i)->DataOffset;
            printf("SamplesPerPixel"); _printData;
            break;

            // ignore 274, 
        case 278:
            // important for finding image data in image
            assert((t->TagList + i)->DataType == 3 || (t->TagList + i)->DataType == 4, "StripOffsets data type wrong\n");
            printf("RowsPerStrip"); _printData;
            t->rowsPerStrip = (t->TagList + i)->DataOffset;
            break;
        case 279:
            //stripByteCounts
            assert((t->TagList + i)->DataType == 3 || (t->TagList + i)->DataType == 4, "StripByteCounts data type wrong\n");
            
            t->stripByteCounts = malloc((t->TagList + i)->DataCount * sizeof(DWORD));
            assertNotNull(t->stripByteCounts, "malloc at stripByteCounts failed\n");
            
            tmpOffset = (t->TagList + i)->DataOffset;
            for (int index = 0; index < (t->TagList + i)->DataCount; index++) {
                if ((t->TagList + i)->DataType == 3){
                    *(t->stripByteCounts + index) = readShortFromBuffer(littleEndian, tmpOffset, buffer, fileSize);
                    tmpOffset += sizeof(short);
                }
                else {
                    *(t->stripByteCounts + index) = readIntFromBuffer(littleEndian, tmpOffset, buffer, fileSize);
                    tmpOffset += sizeof(int);
                }
                printf("StripByteCounts: %d\n", *(t->stripByteCounts + index));
            }
            break;
        case 282:
            assert((t->TagList + i)->DataType == 5, "XResolution data type wrong\n");

            tmpOffset = (t->TagList + i)->DataOffset;
            t->xResolutionNum = readIntFromBuffer(littleEndian, tmpOffset, buffer, fileSize);
            tmpOffset += sizeof(int);
            t->xResolutionDen = readIntFromBuffer(littleEndian, tmpOffset, buffer, fileSize);
            //readIntFromBuffer()

            printf("XResolution: %d / %d\n", t->xResolutionNum, t->xResolutionDen);
            break;
        case 283:
            assert((t->TagList + i)->DataType == 5, "YResolution data type wrong\n");

            tmpOffset = (t->TagList + i)->DataOffset;
            t->yResolutionNum = readIntFromBuffer(littleEndian, tmpOffset, buffer, fileSize);
            tmpOffset += sizeof(int);
            t->yResolutionDen = readIntFromBuffer(littleEndian, tmpOffset, buffer, fileSize);
            //readIntFromBuffer()

            printf("YResolution: %d / %d\n", t->xResolutionNum, t->xResolutionDen);
            break;
        case 284:
            assert((t->TagList + i)->DataType == 3, "Planar Config data type wrong\n");
            // afaik 1 means RGBRGB, 2 means RRGGBB
            assert((t->TagList + i)->DataOffset == 1, "can only deal with RGBRGB, not with RRGGBB\n");
            printf("Planar Config"); _printData;
            break;
        default:
            break;
        }
    }

    return 0;
}

int main(void) {
    printf("size of int = %lu\n", sizeof(int));
    printf("size of short = %lu\n", sizeof(short));

    const char* filename = "spirals/template1M.tiff";

    int sizeofImage = sizeofFile(filename);
    assert(sizeofImage > 0, "");

    // sizeofImage is a byte count and length of buffer
    unsigned char * buffer = malloc(sizeofImage);
    assertNotNull(buffer, "Memory allocation failed\n");

    int result = fileReader(filename, buffer, sizeofImage);
    assert(result == 0, "File Reading Failed\n");

    
    TIFHEAD t;
    result = parseHeader(&t, buffer, sizeofImage);
    assert(result == 0, "Header Parsing Failed\n");
    
    //printf("sizeofImage = %d\n", sizeofImage);
    

    return 0;
}
