#ifndef tiffReader_H_
#define tiffReader_H_

#include "header.h"
#include "tiffImage.h"
#include "tiffTagStorage.h"

bool _tiffReader_isProperHeader(uint16_t identifier, uint16_t version);

int sizeofFile(const char* filename);

int fileReader(const char* filename, unsigned char* buffer, unsigned int fileSize);


//internal functions - not for use outside of tiffReader.c

uint8_t _tiffReader_read1ByteFromBuffer(WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

uint16_t _tiffReader_read2BytesFromBuffer(WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

uint32_t _tiffReader_read4BytesFromBuffer(WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

int _tiffReader_parseFile(tiffFile_t* t, unsigned char* buffer, unsigned int fileSize);

int _tiffReader_parseIFD(tiffImage_t* t, WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize);


#endif
