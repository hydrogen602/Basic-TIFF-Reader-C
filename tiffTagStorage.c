#include <stdlib.h>
#include <stdio.h>
#include "tiffTagStorage.h"
#include "tiffFormat.h"
#include "tiffImage.h"

tiffDataTag_t newDataTag(WORD tagId, WORD dataType, size_t dataCount) {
    tiffDataTag_t tmp;
    tmp.tagId = tagId;
    tmp.dataType = dataType;
    tmp.dataCount = dataCount;
    if (dataType <= 0 || dataType > NUMBER_OF_TYPES) {
        fprintf(stderr, "newDataTag: dataType invalid, got %ud\n", dataType);
        tmp.data = NULL;
    }
    else {
        tmp.data = malloc(getTypeSizeOf(dataType) * dataCount);
    }
    
    return tmp;
}

void freeDataTag(tiffDataTag_t *t) {
    free(t->data);
}

// int main(void) {
//     printf("test\n");
//     tiffDataTag_t dt = dataTag(1, 1, 1);

//     freeDataTag(dt);
// }
