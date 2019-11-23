#ifndef tiffReader_H_
#define tiffReader_H_

#include "tiffImage.h"

bool isProperHeader(tiffHead_t* t);

int sizeofFile(const char* filename);

int fileReader(const char* filename, unsigned char* buffer, unsigned int fileSize);

short readShortFromBuffer(bool littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

int readIntFromBuffer(bool littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

int parseHeader(tiffHead_t* t, unsigned char* buffer, unsigned int fileSize);

int parseIFD(tiffImage_t* t, bool littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

#endif
