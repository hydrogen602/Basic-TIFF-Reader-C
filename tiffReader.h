#ifndef tiffReader_H_
#define tiffReader_H_

#include "header.h"
#include "tiffImage.h"
#include "tiffTagStorage.h"

/*
 * reads a tiff file and puts its content into the tiffFile_t struct
 * Initializes tiffFile_t so use brand new struct;
 * returns 0 upon success, else other
 */
int readFile(const char* filename, tiffFile_t* t);

/*
 * frees all the memory used by a tiffFile_t struct
 * call this when image is no longer in use
 */
void freeFile(tiffFile_t *t);

//internal functions - not for use outside of tiffReader.c

int sizeofFile(const char* filename);

bool _tiffReader_isProperHeader(uint16_t identifier, uint16_t version);

int _tiffReader_fileReader(const char* filename, unsigned char* buffer, unsigned int fileSize);

uint8_t _tiffReader_read1ByteFromBuffer(WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

uint16_t _tiffReader_read2BytesFromBuffer(WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

uint32_t _tiffReader_read4BytesFromBuffer(WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

int _tiffReader_parseFile(tiffFile_t* t, unsigned char* buffer, unsigned int fileSize);

int _tiffReader_parseIFD(tiffImage_t* t, WORD byteOrder, unsigned int offset, unsigned char* buffer, unsigned int fileSize);


#endif
