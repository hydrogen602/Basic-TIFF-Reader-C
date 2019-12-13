#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "tiffReader.h"
#include "tiffTagStorage.h"
#include "tiffImage.h"
#include "tiffReaderHelper.h"
#include "support/array.h"

// apparently macros can be define with -D name

bool _tiffReader_isProperHeader(uint16_t identifier, uint16_t version) {

    if (identifier != TIFF_BIG_ENDIAN && identifier != TIFF_LITTLE_ENDIAN) {
        return false;
    }
    if (version != TIFF_VERSION && version != TIFF_VERSION_BYTEFLIP) {
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

int _tiffReader_fileReader(const char* filename, unsigned char* buffer, unsigned int fileSize) {
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

/* ======== <helper read functions> ======== */

uint8_t _tiffReader_read1ByteFromBuffer(WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    if (offset + sizeof(uint8_t) - 1 >= fileSize) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException: %lu at line %d\n", offset + sizeof(uint8_t), __LINE__);
        fprintf(stderr, "byteOrder = %s, offset = %u, fileSize = %u\n", (byteOrder == TIFF_LITTLE_ENDIAN) ? "little" : "big", offset, fileSize);
        exit(1);
    }

    uint8_t result = *(buffer + offset);
    // no byteswapping to do cause it's one byte

    return result;   
}

uint16_t _tiffReader_read2BytesFromBuffer(WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    if (offset + sizeof(uint16_t) - 1 >= fileSize) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException: %lu at line %d\n", offset + sizeof(uint16_t), __LINE__);
        exit(1);
    }

    uint16_t result;
    if (byteOrder == TIFF_LITTLE_ENDIAN) {
        memcpy(&result, buffer + offset, sizeof(uint16_t));
    }
    else if (byteOrder == TIFF_BIG_ENDIAN) {
        uint16_t tmp;
        memcpy(&tmp, buffer + offset, sizeof(uint16_t));
        result = __builtin_bswap16(tmp);        
    }
    else {
        fprintf(stderr, "Invalid State: byteOrder not correct: %ud\n", byteOrder);
        result = 0;
    }

    return result;
}

uint32_t _tiffReader_read4BytesFromBuffer(WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    // we start reading at offset, so it is sizeof( type ) - 1
    if (offset + sizeof(uint32_t) - 1 >= fileSize) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException: %lu at line %d\n", offset + sizeof(uint32_t), __LINE__);
        exit(1);
    }

    uint32_t result;
    if (byteOrder == TIFF_LITTLE_ENDIAN) {
        memcpy(&result, buffer + offset, sizeof(uint32_t));
    }
    else if (byteOrder == TIFF_BIG_ENDIAN) {
        uint32_t tmp;
        memcpy(&tmp, buffer + offset, sizeof(uint32_t));
        result = __builtin_bswap32(tmp);        
    }
    else {
        fprintf(stderr, "Invalid State: byteOrder not correct: %ud\n", byteOrder);
        result = 0;
    }

    return result;
}

uint64_t _tiffReader_read8BytesFromBuffer(WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    if (offset + sizeof(uint64_t) - 1 >= fileSize) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException: %lu at line %d\n", offset + sizeof(uint64_t), __LINE__);
        exit(1);
    }

    uint64_t result;
    if (byteOrder == TIFF_LITTLE_ENDIAN) {
        memcpy(&result, buffer + offset, sizeof(uint64_t));
    }
    else if (byteOrder == TIFF_BIG_ENDIAN) {
        uint64_t tmp;
        memcpy(&tmp, buffer + offset, sizeof(uint64_t));
        result = __builtin_bswap64(tmp);        
    }
    else {
        fprintf(stderr, "Invalid State: byteOrder not correct: %ud\n", byteOrder);
        result = 0;
    }

    return result;
}

/* ======== </helper read functions> ======== */


int _tiffReader_parseFile(tiffFile_t* t, unsigned char* buffer, unsigned int fileSize) {
    // will probably only compile with gcc

    /*
     * tiff header
     */

    // basic tiff header is 8 bytes long
    assert(fileSize >= 8, "Image doesn't even have a header\n");

    unsigned int positionInMemory = 0;

    WORD byteOrder;
    memcpy(&byteOrder, buffer, sizeof(WORD));
    positionInMemory += sizeof(WORD);

    WORD version;
    memcpy(&version, buffer + positionInMemory, sizeof(WORD));
    positionInMemory += sizeof(WORD);

    assert(_tiffReader_isProperHeader(byteOrder, version), "Image doesn't have a proper header\n");

    t->byteOrder = byteOrder;
    t->images = NULL;
    t->imagesCount = 0; // not known yet cause they are stored linked-list style

    /* 
     * runs with the assumption of being run on a little-endian machine 
     * cause I don't want to kill myself over byte order 
     */

    DWORD ifdOffset = _tiffReader_read4BytesFromBuffer(byteOrder, positionInMemory, buffer, fileSize);
    positionInMemory += sizeof(int);
    
    if (t->byteOrder == TIFF_BIG_ENDIAN) {
        #ifdef DEBUG    
        printf("got order: big endian\n");
        printf("got version: %x\n", __builtin_bswap16(version));
        #endif
    }
    else if (t->byteOrder == TIFF_LITTLE_ENDIAN) {
        #ifdef DEBUG
        printf("got order: little endian\n");
        printf("got version: %x\n", version);
        #endif
    }
    else {
        assertEx(false, "Invalid State: byteOrder is not correct: %ud\n", t->byteOrder);
    }

    #ifdef DEBUG
    printf("got offset: %x\n", ifdOffset);
    #endif

    t->imagesCount = 1; // only supports 1 image per file for now
    t->images = malloc(sizeof(tiffImage_t) * (t->imagesCount));

    for (int i = 0; i < t->imagesCount; ++i) {
        t->images[i] = makeImage(RGB);
    }

    int result = _tiffReader_parseIFD(t->images + 0, byteOrder, ifdOffset, buffer, fileSize);

    assert(result == 0, "_tiffReader_parseIFD failed\n");

    return 0;
}

int _tiffReader_parseIFD(tiffImage_t* t, WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    t->height = 0;
    t->width = 0;

    // offset + buffer should be where the IFD is
    size_t tagCount = _tiffReader_read2BytesFromBuffer(byteOrder, offset, buffer, fileSize);
    if (tagCount == 0 || tagCount >= 100) {
        fprintf(stderr, "Got an unusual amount of tags: %ld\n", tagCount);
    }

    offset += sizeof(short);

    // remember to free this memory, len = t.NumDirEntries
    t->tags = newExpandableArray(tagCount, sizeof(tiffDataTag_t));

    assertNotNull(t->tags, "Failed to allocate memory\n");

    #ifdef DEBUG
    printf("Number of tags: %lu\n", tagCount);
    printf("Offset: %d\n", offset);
    #endif

    for (int i = 0; i < tagCount; i++) {
        tiffDataTag_t * tagPtr = t->tags + i;

        WORD tagId = _tiffReader_read2BytesFromBuffer(byteOrder, offset, buffer, fileSize);
        #ifdef DEBUG
        printf("TagId: %d\n", tagId);
        #endif
        offset += sizeof(WORD);
        
        WORD dataType = _tiffReader_read2BytesFromBuffer(byteOrder, offset, buffer, fileSize);
        offset += sizeof(WORD);

        DWORD dataCount = _tiffReader_read4BytesFromBuffer(byteOrder, offset, buffer, fileSize);
        offset += sizeof(DWORD);

        // create data tag
        // NOTE: dataTag calls malloc, must be freed with freeDataTag()
        *(tagPtr) = newDataTag(tagId, dataType, dataCount);
        
        #ifdef DEBUG
        printf("Now creating data tag with id=%u, dataType=%u, dataCount=%u\n", tagId, dataType, dataCount);
        #endif

        /* === get data === */

        // assume that if sizeof data is < 4 bytes and there is only one piece of data, the data will be stored in DataOffset
        
        // two bytes types
        if ((dataType == SHORT_TypeID || dataType == SSHORT_TypeID) && tagPtr->dataCount == 1) {
            uint16_t n = _tiffReader_read2BytesFromBuffer(byteOrder, offset, buffer, fileSize);
            
            set(n, tagPtr, 0);

            #ifdef DEBUG
            if (tagPtr->tagId == 259) {
                printf("addr = %x\n", tagPtr->data);
                short n2;
                memcpy(&n2, tagPtr->data, sizeof(short));
                printf("should: %d == %u\n", n2, n);
            }

            printf("%u\n", n);
            #endif

            if (tagPtr->tagId == ImageHeight) {
                t->height = n;
            }
            else if (tagPtr->tagId == ImageWidth) {
                t->width = n;
            }
        }

        // four bytes types
        else if ((dataType == LONG_TypeID || dataType == SLONG_TypeID) && tagPtr->dataCount == 1) {
            uint32_t n = _tiffReader_read4BytesFromBuffer(byteOrder, offset, buffer, fileSize);

            set(n, tagPtr, 0);

            #ifdef DEBUG
            printf("%u\n", n);
            #endif

            // image height and width may be two or four bytes
            if (tagPtr->tagId == ImageHeight) {
                t->height = n;
            }
            else if (tagPtr->tagId == ImageWidth) {
                t->width = n;
            }
        }

        // one byte types
        else if ((dataType == BYTE_TypeID || dataType == SBYTE_TypeID || dataType == UNDEFINE_TypeID) && tagPtr->dataCount == 1) {
            uint8_t n = _tiffReader_read1ByteFromBuffer(byteOrder, offset, buffer, fileSize);
            set(n, tagPtr, 0);

            #ifdef DEBUG
            printf("%u\n", n);
            #endif
        }

        // data located elsewhere
        else {
            DWORD tmpOffset = _tiffReader_read4BytesFromBuffer(byteOrder, offset, buffer, fileSize);

            #ifdef DEBUG
            if (tagPtr->tagId == 34675) { // embeded color profile
                printf("INFO:\n");
                printf("\tsizeof(data) = %lu\n", getTypeSizeOf(tagPtr->dataType));
                printf("\tdata count   = %lu\n", tagPtr->dataCount);
                printf("\ttmpOffset    = %u\n", tmpOffset);
                printf("\tfileSize     = %u\n", fileSize);
                putchar('\n');

            }

            printf("data point = ");
            #endif

            for (int index = 0; index < tagPtr->dataCount; ++index) {
                /* the formats and their sizes
                    sizeof(BYTE),       // 1
                    sizeof(ASCII),      // 1
                    sizeof(SHORT),      // 2
                    sizeof(LONG),       // 4
                    sizeof(RATIONAL),   // 8 (4 + 4)
                    sizeof(SBYTE),      // 1
                    sizeof(UNDEFINE),   // 1
                    sizeof(SSHORT),     // 2
                    sizeof(SLONG),      // 4
                    sizeof(SRATIONAL),  // 8 (4 + 4)
                    sizeof(FLOAT),      // 4
                    sizeof(DOUBLE),     // 8
                */

                if (getTypeSizeOf(dataType) == 1) {
                   uint8_t n = _tiffReader_read1ByteFromBuffer(byteOrder, tmpOffset, buffer, fileSize);

                   #ifdef DEBUG
                   if (dataType == ASCII_TypeID || dataType == BYTE_TypeID) {
                       printf("%c", n);
                   }
                   else {
                       printf("%u, ", n);
                   }
                   #endif
                   
                   set(n, tagPtr, index);
                }

                else if (getTypeSizeOf(dataType) == 2) {
                   uint16_t n = _tiffReader_read2BytesFromBuffer(byteOrder, tmpOffset, buffer, fileSize);

                   #ifdef DEBUG
                   printf("%u, ", n);
                   #endif

                    set(n, tagPtr, index);
                }

                else if (getTypeSizeOf(dataType) == 4) {
                   uint32_t n = _tiffReader_read4BytesFromBuffer(byteOrder, tmpOffset, buffer, fileSize);

                    #ifdef DEBUG
                    printf("%u, ", n);
                    #endif

                    set(n, tagPtr, index);
                }

                else if (dataType == RATIONAL_TypeID || dataType == SRATIONAL_TypeID) {
                    RATIONAL r; // eh whatever the sign
                    r.num = _tiffReader_read4BytesFromBuffer(byteOrder, tmpOffset, buffer, fileSize);
                    r.denom = _tiffReader_read4BytesFromBuffer(byteOrder, tmpOffset + sizeof(DWORD), buffer, fileSize);

                    #ifdef DEBUG
                    printf("%u / %u, ", r.num, r.denom);
                    #endif

                    set(r, tagPtr, index);
                }
                else if (dataType == DOUBLE_TypeID) {
                    uint64_t n = _tiffReader_read8BytesFromBuffer(byteOrder, tmpOffset, buffer, fileSize);

                    #ifdef DEBUG
                    printf("%lld, ", n);
                    #endif

                    set(n, tagPtr, index);
                }

                tmpOffset += getTypeSizeOf(dataType);
            }
            #ifdef DEBUG
            putchar('\n');
            #endif
        }
        #ifdef DEBUG
        tagPrintDebug(tagPtr);
        #endif

        offset += sizeof(DWORD);
    }

    if (t->height == 0 || t->width == 0) {
        printErrMsg("Could not find height or width\n");
        return -1;
    }

    printf("Read Image\n");
    printf("\theight = %ld\n", t->height);
    printf("\twidth = %ld\n", t->width);
    putchar('\n');
    int result = stripsReaderFunc(t, buffer, fileSize);
    if (result != 0) {
        printErrMsg("stripsReaderFunc failed\n");
        return -1;
    }

    return 0;
}

void _tiffReader_freeIFD(tiffImage_t* t) {
    for (size_t i = 0; i < len(t->tags); ++i) {
        freeDataTag(t->tags + i);
    }
    if (t->pixels != NULL) { free(t->pixels); }
    if (t->pixelsRed != NULL) { free(t->pixelsRed); }
    if (t->pixelsGreen!= NULL) { free(t->pixelsGreen); }
    if (t->pixelsBlue != NULL) { free(t->pixelsBlue); }
    
    free(t->tags);
}

void freeFile(tiffFile_t *t) {
    for (size_t i = 0; i < t->imagesCount; ++i) {
        _tiffReader_freeIFD(t->images + i);
    }
    free(t->images);
}

int readFile(const char* filename, tiffFile_t* t) {

    t->byteOrder = TIFF_BIG_ENDIAN;
    t->images = NULL;
    t->imagesCount = 0;

    int sizeofImage = sizeofFile(filename);
    if (sizeofImage == -1) { printErrMsg("readFile failed"); return -1; }
    assert(sizeofImage > 0, "image not a real size");

    // sizeofImage is a byte count and length of buffer
    unsigned char* buffer = malloc(sizeofImage);
    assertNotNull(buffer, "Memory allocation failed\n");

    int result = _tiffReader_fileReader(filename, buffer, sizeofImage);
    assert(result == 0, "File Reading Failed\n");

    
    result = _tiffReader_parseFile(t, buffer, sizeofImage);
    assert(result == 0, "File Parsing Failed\n");
    
    //printf("sizeofImage = %d\n", sizeofImage);
    
    free(buffer);

    return 0; // remember to call freeFile(&t);
}
