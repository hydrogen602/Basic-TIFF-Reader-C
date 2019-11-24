#ifndef tiffReader_H_
#define tiffReader_H_

#include "tiffImage.h"

bool isProperHeader(tiffHead_t* t);

int sizeofFile(const char* filename);

int fileReader(const char* filename, unsigned char* buffer, unsigned int fileSize);

uint8_t read1ByteFromBuffer(bool littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

uint16_t read2BytesFromBuffer(bool littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

uint32_t read4BytesFromBuffer(bool littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

int parseHeader(tiffHead_t* t, unsigned char* buffer, unsigned int fileSize);

int parseIFD(tiffImage_t* t, bool littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

#endif
