#ifndef TIFF_TAG_STORAGE_H_
#define TIFF_TAG_STORAGE_H_

#include <stdlib.h>
#include "header.h"
#include "tiffImage.h"

#define indexData(dataTag, index) *(((dataTag)->data) + ((index) * getTypeSizeOf((dataTag)->dataType)))

// https://www.fileformat.info/format/tiff/egff.htm

// creates new dataTag and allocates memory for data
tiffDataTag_t newDataTag(WORD tagId, WORD dataType, size_t dataCount);

// frees allocated memory of dataTag
void freeDataTag(tiffDataTag_t *t);

#endif
