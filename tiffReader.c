#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "tiffReader.h"

bool isProperHeader(tiffHead_t* t) {
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

uint8_t read1ByteFromBuffer(bool littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    if (offset + sizeof(uint8_t) >= fileSize) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException: %lu\n", offset + sizeof(uint8_t));
        exit(1);
    }

    uint8_t result = *(buffer + offset);
    // no byteswapping to do cause it's one byte

    return result;   
}

uint16_t read2BytesFromBuffer(bool littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    if (offset + sizeof(uint16_t) >= fileSize) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException: %lu\n", offset + sizeof(uint16_t));
        exit(1);
    }

    uint16_t result;
    if (littleEndian) {
        memcpy(&result, buffer + offset, sizeof(uint16_t));
    }
    else {
        uint16_t tmp;
        memcpy(&tmp, buffer + offset, sizeof(uint16_t));
        result = __builtin_bswap16(tmp);        
    }

    return result;
}

uint32_t read4BytesFromBuffer(bool littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    if (offset + sizeof(uint32_t) >= fileSize) {
        fprintf(stderr, "ArrayIndexOutOfBoundsException: %lu\n", offset + sizeof(uint32_t));
        exit(1);
    }

    uint32_t result;
    if (littleEndian) {
        memcpy(&result, buffer + offset, sizeof(uint32_t));
    }
    else {
        uint32_t tmp;
        memcpy(&tmp, buffer + offset, sizeof(uint32_t));
        result = __builtin_bswap32(tmp);        
    }

    return result;
}

int parseHeader(tiffHead_t* t, unsigned char* buffer, unsigned int fileSize) {
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

    bool littleEndian = -1;
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

    parseIFD(&(t->IFDOffset), littleEndian, t->IFDOffset, buffer, fileSize);

    return 0;
}

void freeIFD(tiffImage_t* t) {
    for (size_t i = 0; i < t->tagCount; ++i) {
        freeDataTag(t->tags + i);
    }
}

int parseIFD(tiffImage_t* t, bool littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize) {
    // offset + buffer should be where the IFD is
    t->tagCount = readShortFromBuffer(littleEndian, offset, buffer, fileSize);
    offset += sizeof(short);

    printf("Number of tags: %d\n", t->tagCount);

    // remember to free this memory, len = t.NumDirEntries
    t->tags = malloc(t->tagCount * sizeof(tiffDataTag_t));
    assertNotNull(t->tags, "Failed to allocate memory\n");

    printf("Offset: %d\n", offset);

    for (int i = 0; i < t->tagCount; i++) {

        WORD tagId = readShortFromBuffer(littleEndian, offset, buffer, fileSize);
        printf("TagId: %d\n", tagId);
        offset += sizeof(WORD);
        
        WORD dataType = readShortFromBuffer(littleEndian, offset, buffer, fileSize);
        offset += sizeof(WORD);

        DWORD dataCount = readIntFromBuffer(littleEndian, offset, buffer, fileSize);
        offset += sizeof(DWORD);

        // create data tag
        // NOTE: dataTag calls malloc, must be freed with freeDataTag()
        *(t->tags + i) = newDataTag(tagId, dataType, dataCount);

        /* === get data === */

        // assume that if sizeof data is < 4 bytes and there is only one piece of data, the data will be stored in DataOffset
        
        // two bytes types
        if ((dataType == SHORT_TypeID || dataType == SSHORT_TypeID) && (t->tags + i)->dataCount == 1) {
            indexData(t->tags + i, 0) = readShortFromBuffer(littleEndian, offset, buffer, fileSize);
        }

        // four bytes types
        else if ((dataType == LONG_TypeID || dataType == SLONG_TypeID) && (t->tags + i)->dataCount == 1) {
            indexData(t->tags + i, 0) = readIntFromBuffer(littleEndian, offset, buffer, fileSize);
        }

        // one byte types
        else if ((dataType == BYTE_TypeID || dataType == SBYTE_TypeID || dataType == UNDEFINE_TypeID) && (t->tags + i)->dataCount == 1) {
            indexData(t->tags + i, 0) = readByteFromBuffer(littleEndian, offset, buffer, fileSize);
        }

        // data located elsewhere
        else {
            DWORD jmpAddress = readIntFromBuffer(littleEndian, offset, buffer, fileSize);
        }

        offset += sizeof(DWORD);

        
        
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

    
    tiffHead_t t;
    result = parseHeader(&t, buffer, sizeofImage);
    assert(result == 0, "Header Parsing Failed\n");
    
    //printf("sizeofImage = %d\n", sizeofImage);
    

    return 0;
}
