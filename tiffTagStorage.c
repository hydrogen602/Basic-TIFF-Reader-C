#include <stdlib.h>
#include <stdio.h>
#include "tiffTagStorage.h"
#include "tiffFormat.h"

tiffDataTag_t dataTag(WORD tagId, WORD dataType, size_t dataCount) {
    tiffDataTag_t tmp;
    tmp.tagId = tagId;
    tmp.dataType = dataType;
    tmp.dataTypeSize = sizeOfTypeLookupTable[dataType];
    tmp.dataCount = dataCount;
    tmp.data = malloc(tmp.dataTypeSize * dataCount);
    return tmp;
}

void freeDataTag(tiffDataTag_t t) {
    free(t.data);
}

int main(void) {
    printf("test\n");
    tiffDataTag_t dt = dataTag(1, 1, 1);

    freeDataTag(dt);
}
