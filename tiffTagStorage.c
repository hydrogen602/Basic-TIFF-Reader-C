#include <stdlib.h>

#include "tiffTagStorage.h"

tiffDataTag_t dataTag(WORD tagId, WORD dataType, size_t dataTypeSize, size_t dataCount) {
    tiffDataTag_t tmp;
    tmp.tagId = tagId;
    tmp.dataType = dataType;
    tmp.dataTypeSize = dataTypeSize;
    tmp.dataCount = dataCount;
    tmp.data = malloc(dataTypeSize * dataCount);
    return tmp;
}

void freeDataTag(tiffDataTag_t t) {
    free(t.data);
}
