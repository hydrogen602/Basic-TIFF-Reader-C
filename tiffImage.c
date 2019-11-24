#include <stdlib.h>
#include <stdio.h>

#include "header.h"
#include "tiffImage.h"
#include "tiffTagStorage.h"
#include "tiffFormat.h"

// index i should return sizeof type i
const unsigned long SIZEOF_TYPE_LOOKUP_TABLE[13] = { 0, 
    sizeof(BYTE),
    sizeof(ASCII),
    sizeof(SHORT),
    sizeof(LONG),
    sizeof(RATIONAL),
    sizeof(SBYTE),
    sizeof(UNDEFINE),
    sizeof(SSHORT),
    sizeof(SLONG),
    sizeof(SRATIONAL),
    sizeof(FLOAT),
    sizeof(DOUBLE),
};

const int required_gray_tags[] = GRAY_SCALE_TAGS;
const int required_rgb_tags[] = RGB_TAGS;

size_t getTypeSizeOf(int typeId) {
    assertEx(typeId > 0 && typeId <= NUMBER_OF_TYPES, "ArrayIndexOutOfBoudsException: %d\n", typeId);
    
    return SIZEOF_TYPE_LOOKUP_TABLE[typeId];
}

tiffImage_t makeImage(imgType iType) {
    tiffImage_t img;

    img.pixelCount = 0;

    img.pixels = NULL;

    img.pixelsBlue = NULL;
    img.pixelsGreen = NULL;
    img.pixelsRed = NULL;

    img.type = iType;
    if (iType == RGB || iType == GRAY_SCALE) {
        // pass, nothing to do
    }
    else {
        fprintf(stderr, "Invalid argument in makeImage, got %d\n", iType);
        img.type = -1;
    }

    img.nextIFDOffset = 0; // deal with later

    img.tagCount = 0;
    img.tags = NULL;
    
    return img;
}

bool _tiffImage_isValidImage_helper(const int * requiredTags, size_t requiredTagLength, tiffDataTag_t * actualTags, size_t actualLength) {
    for (unsigned long t = 0; t < requiredTagLength; ++t) {
        bool found = false;
        for (unsigned long i = 0; i < actualLength; ++i) {
            if ((actualTags + i)->tagId == requiredTags[t]) {
                found = true;
                break;
            }
        }
        if (!found) {
            return false;
        }
    }
    return true;
}

bool isValidImage(tiffImage_t* img) {
    if (img->type == GRAY_SCALE) {
        if (!_tiffImage_isValidImage_helper(required_gray_tags, GRAY_SCALE_TAGS_COUNT, img->tags, img->tagCount)) {
            return false;
        }

        if (img->pixels == NULL) return false;
        if (img->pixelsBlue != NULL) return false;
        if (img->pixelsGreen != NULL) return false;
        if (img->pixelsRed != NULL) return false;
    }
    else if (img->type == RGB) {
        if (!_tiffImage_isValidImage_helper(required_rgb_tags, RGB_TAGS_COUNT, img->tags, img->tagCount)) {
            return false;
        }

        if (img->pixels != NULL) return false;
        if (img->pixelsBlue == NULL) return false;
        if (img->pixelsGreen == NULL) return false;
        if (img->pixelsRed == NULL) return false;
    }
    else {
        fprintf(stderr, "Invalid image type, got %d\n", img->type);
        return false;
    }

    return true;
}

tiffFile_t makeFile(tiffImage_t* images, size_t imagesCount) {
    tiffFile_t file;

    file.byteOrder = TIFF_BIG_ENDIAN; // why this byte order? Because why not?

    file.images = images;
    file.imagesCount = imagesCount;

    return file;
}


