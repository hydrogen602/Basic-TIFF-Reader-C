#ifndef TIFF_TAG_STORAGE_H_
#define TIFF_TAG_STORAGE_H_

#include <stdlib.h>
#include <string.h>
#include "header.h"
#include "tiffImage.h"

//#define indexData(dataTag, index) *(((dataTag)->data) + ((index) * getTypeSizeOf((dataTag)->dataType)))
#define indexDataPtr(dataTag, index) (((dataTag)->data) + ((index) * getTypeSizeOf((dataTag)->dataType)))

// memcpy(&n, t->data, sizeof(short));
#define get(n, tag, index) (n)=0; memcpy(&(n), indexDataPtr(tag, index), getTypeSizeOf((tag)->dataType))
// the n = 0 is necessary as memcpy might only set parts of the variable, allowing garbage to remain as the var isn't fully initialized
#define set(n, tag, index) memcpy(indexDataPtr(tag, index), &(n), getTypeSizeOf((tag)->dataType))

// https://www.fileformat.info/format/tiff/egff.htm

// creates new dataTag and allocates memory for data
tiffDataTag_t newDataTag(WORD tagId, WORD dataType, size_t dataCount);

// searches through an array of tags for a particular tag. Returns a pointer to the tag or null
tiffDataTag_t* findTag(WORD tagId, tiffDataTag_t* tags, size_t tagCount);

// frees allocated memory of dataTag
void freeDataTag(tiffDataTag_t *t);

// print tag name if known
void tagPrintDebug(tiffDataTag_t *t);

#endif
