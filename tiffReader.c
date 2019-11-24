#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "tiffReader.h"
#include "tiffTagStorage.h"
#include "tiffImage.h"

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

/* ======== <helper read functions> ======== */

uint8_t _tiffReader_read1ByteFromBuffer(WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    if (offset + sizeof(uint8_t) >= fileSize) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException: %lu\n", offset + sizeof(uint8_t));
        exit(1);
    }

    uint8_t result = *(buffer + offset);
    // no byteswapping to do cause it's one byte

    return result;   
}

uint16_t _tiffReader_read2BytesFromBuffer(WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    if (offset + sizeof(uint16_t) >= fileSize) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException: %lu\n", offset + sizeof(uint16_t));
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
    if (offset + sizeof(uint32_t) >= fileSize) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException: %lu\n", offset + sizeof(uint32_t));
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
    memcpy(&version, buffer, sizeof(WORD));
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
        printf("got order: big endian\n");
    }
    else if (t->byteOrder == TIFF_LITTLE_ENDIAN) {
        printf("got order: little endian\n");
    }
    else {
        assertEx(false, "Invalid State: byteOrder is not correct: %ud\n", t->byteOrder);
    }

    printf("got version: %x\n", version);
    printf("got offset: %x\n", ifdOffset);

    //_tiffReader_parseIFD(&(t->IFDOffset), littleEndian, t->IFDOffset, buffer, fileSize);

    return 0;
}

int _tiffReader_parseIFD(tiffImage_t* t, WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    // offset + buffer should be where the IFD is
    t->tagCount = _tiffReader_read2BytesFromBuffer(byteOrder, offset, buffer, fileSize);
    offset += sizeof(short);

    printf("Number of tags: %lu\n", t->tagCount);

    // remember to free this memory, len = t.NumDirEntries
    t->tags = malloc(t->tagCount * sizeof(tiffDataTag_t));
    assertNotNull(t->tags, "Failed to allocate memory\n");

    printf("Offset: %d\n", offset);

    for (int i = 0; i < t->tagCount; i++) {

        WORD tagId = _tiffReader_read2BytesFromBuffer(byteOrder, offset, buffer, fileSize);
        printf("TagId: %d\n", tagId);
        offset += sizeof(WORD);
        
        WORD dataType = _tiffReader_read2BytesFromBuffer(byteOrder, offset, buffer, fileSize);
        offset += sizeof(WORD);

        DWORD dataCount = _tiffReader_read4BytesFromBuffer(byteOrder, offset, buffer, fileSize);
        offset += sizeof(DWORD);

        // create data tag
        // NOTE: dataTag calls malloc, must be freed with freeDataTag()
        *(t->tags + i) = newDataTag(tagId, dataType, dataCount);

        /* === get data === */

        // assume that if sizeof data is < 4 bytes and there is only one piece of data, the data will be stored in DataOffset
        
        // two bytes types
        if ((dataType == SHORT_TypeID || dataType == SSHORT_TypeID) && (t->tags + i)->dataCount == 1) {
            indexData(t->tags + i, 0) = _tiffReader_read2BytesFromBuffer(byteOrder, offset, buffer, fileSize);
        }

        // four bytes types
        else if ((dataType == LONG_TypeID || dataType == SLONG_TypeID) && (t->tags + i)->dataCount == 1) {
            indexData(t->tags + i, 0) = _tiffReader_read4BytesFromBuffer(byteOrder, offset, buffer, fileSize);
        }

        // one byte types
        else if ((dataType == BYTE_TypeID || dataType == SBYTE_TypeID || dataType == UNDEFINE_TypeID) && (t->tags + i)->dataCount == 1) {
            indexData(t->tags + i, 0) = _tiffReader_read1ByteFromBuffer(byteOrder, offset, buffer, fileSize);
        }

        // data located elsewhere
        else {
            DWORD jmpAddress = _tiffReader_read4BytesFromBuffer(byteOrder, offset, buffer, fileSize);
        }

        offset += sizeof(DWORD);

        
        
    }

    return 0;
}

void _tiffReader_freeIFD(tiffImage_t* t) {
    for (size_t i = 0; i < t->tagCount; ++i) {
        freeDataTag(t->tags + i);
    }
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

    
    tiffFile_t t;
    result = _tiffReader_parseFile(&t, buffer, sizeofImage);
    assert(result == 0, "File Parsing Failed\n");
    
    //printf("sizeofImage = %d\n", sizeofImage);
    

    return 0;
}
