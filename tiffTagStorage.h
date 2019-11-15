#ifndef TIFF_TAG_STORAGE_H_
#define TIFF_TAG_STORAGE_H_

#include <stdlib.h>

// Defines how to store tiff tags in memory

typedef unsigned short WORD; // 2 bytes
typedef unsigned int DWORD; // 4 bytes

typedef struct _TifTagData
{
    WORD tagId;          /* The tag identifier  */
    WORD dataType;       // type id
    size_t dataTypeSize; // size of type
    size_t dataCount;    // length of array
    unsigned char *data; // data, use malloc & free, kinda like a void ptr
    // but void ptrs don't like arithmetic being done on them and thus its a unsigned char
} tiffDataTag_t;

typedef struct _TifIfdData
{
    size_t dataCount;    // length of tag array
    tiffDataTag_t* tags;  // tag array
} tiffIFDTag_t;

/* === Tiff Tag Data as found in file === */

// https://www.fileformat.info/format/tiff/egff.htm

// typedef struct _TifTag
// {
// 	WORD    tagId;       /* The tag identifier  */
// 	WORD    dataType;    /* The scalar type of the data items  */
// 	DWORD   dataCount;   /* The number of items in the tag data  */
// 	DWORD   dataOffset;  /* The byte offset to the data items  */
// } TIFTAG;

// typedef struct _TifIfd
// {
// 	WORD    numDirEntries;    /* Number of Tags in IFD  */
// 	TIFTAG* tagList;           /* Array of Tags  */
// 	DWORD   nextIFDOffset;    /* Offset to next IFD  */
// } TIFIFD;

typedef struct _TiffHeader
{
	WORD    identifier;  /* Byte-order Identifier */
	WORD    version;     /* TIFF version number (always 2Ah) */
	DWORD   IFDOffset;   /* Offset of the first Image File Directory */
} tiffHead_t;

// functions def

// new dataTag and allocates memory for data
tiffDataTag_t dataTag(WORD tagId, WORD dataType, size_t dataTypeSize, size_t dataCount);

// frees allocated memory of dataTag
void freeDataTag(tiffDataTag_t t);

#endif
