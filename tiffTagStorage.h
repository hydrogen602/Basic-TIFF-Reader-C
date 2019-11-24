#ifndef TIFF_TAG_STORAGE_H_
#define TIFF_TAG_STORAGE_H_

#include <stdlib.h>

#define indexData(dataTag, index) *(((dataTag)->data) + ((index) * SIZEOF_TYPE_LOOKUP_TABLE[(dataTag)->dataType]))

// Defines how to store tiff tags in memory

typedef uint16_t WORD; // 2 bytes
typedef uint32_t DWORD; // 4 bytes


typedef struct _TifTagData
{
    WORD tagId;          /* The tag identifier  */
    WORD dataType;       // type id
    size_t dataCount;    // length of array

    // data, use malloc & free, kinda like a void ptr
    unsigned char *data; 
    // but void ptrs don't like arithmetic being done on them and thus its a unsigned char
} tiffDataTag_t;

// https://www.fileformat.info/format/tiff/egff.htm

// defines a tiff header
typedef struct _TiffHeader
{
	WORD    identifier;  /* Byte-order Identifier */
	WORD    version;     /* TIFF version number (always 2Ah) */
	DWORD   IFDOffset;   /* Offset of the first Image File Directory */
} tiffHead_t;

// functions def

// creates new dataTag and allocates memory for data
tiffDataTag_t newDataTag(WORD tagId, WORD dataType, size_t dataCount);

// frees allocated memory of dataTag
void freeDataTag(tiffDataTag_t *t);

#endif
