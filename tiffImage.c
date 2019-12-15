#include <stdlib.h>
#include <stdio.h>

#include "header.h"
#include "tiffImage.h"
#include "tiffTagStorage.h"
#include "tiffFormat.h"
#include "support/array.h"

// index i should return sizeof type i
const unsigned long SIZEOF_TYPE_LOOKUP_TABLE[13] = { 0, 
    sizeof(BYTE),       // 1
    sizeof(ASCII),      // 1
    sizeof(SHORT),      // 2
    sizeof(LONG),       // 4
    sizeof(RATIONAL),   // 8 (4 + 4)
    sizeof(SBYTE),      // 1
    sizeof(UNDEFINE),   // 1
    sizeof(SSHORT),     // 2
    sizeof(SLONG),      // 4
    sizeof(SRATIONAL),  // 8 (4 + 4)
    sizeof(FLOAT),      // 4
    sizeof(DOUBLE),     // 8
};

const int required_gray_tags[] = GRAY_SCALE_TAGS;
const int required_rgb_tags[] = RGB_TAGS;

size_t getTypeSizeOf(int typeId) {
    if (typeId <= 0 && typeId > NUMBER_OF_TYPES) {
        fprintf(stderr, "ArrayIndexOutOfBoudsException: %d at %d in %s\n", typeId, __LINE__, __FILE__);
        exit(EXIT_FAILURE);
    }
    
    return SIZEOF_TYPE_LOOKUP_TABLE[typeId];
}

tiffImage_t newImage(imgType iType) {
    tiffImage_t img;

    img.pixelCount = 0;

    img.pixels = NULL;

    img.pixelsBlue = NULL;
    img.pixelsGreen = NULL;
    img.pixelsRed = NULL;

    img.height = 0;
    img.width = 0;

    img.type = iType;
    if (iType == RGB || iType == GRAY_SCALE) {
        // pass, nothing to do
    }
    else {
        fprintf(stderr, "Invalid argument in newImage, got %d\n", iType);
        img.type = -1;
    }

    img.nextIFDOffset = 0; // deal with later

    img.tags = newExpandableArray(0, sizeof(tiffDataTag_t));
    
    return img;
}

bool _tiffImage_isValidImage_helper(const int * requiredTags, size_t requiredTagLength, tiffDataTag_t * actualTags) {
    for (unsigned long t = 0; t < requiredTagLength; ++t) {
        bool found = false;
        for (unsigned long i = 0; i < len(actualTags); ++i) {
            if ((actualTags + i)->tagId == requiredTags[t]) {
                found = true;
                break;
            }
        }
        if (!found) {
            fprintf(stderr, "Missing data tag %d\n", requiredTags[t]);
            return false;
        }
    }
    return true;
}

bool isValidImage(tiffImage_t* img) {
    if (img->type == GRAY_SCALE) {
        if (!_tiffImage_isValidImage_helper(required_gray_tags, GRAY_SCALE_TAGS_COUNT, img->tags)) {
            return false;
        }

        if (img->pixels == NULL) return false;
        if (img->pixelsBlue != NULL) return false;
        if (img->pixelsGreen != NULL) return false;
        if (img->pixelsRed != NULL) return false;

        if (len(img->pixels) != img->height * img->width) return false;
    }
    else if (img->type == RGB) {
        if (!_tiffImage_isValidImage_helper(required_rgb_tags, RGB_TAGS_COUNT, img->tags)) {
            return false;
        }

        if (img->pixels != NULL) return false;
        if (img->pixelsBlue == NULL) return false;
        if (img->pixelsGreen == NULL) return false;
        if (img->pixelsRed == NULL) return false;

        if (len(img->pixelsBlue) != img->height * img->width) return false;
        if (len(img->pixelsGreen) != img->height * img->width) return false;
        if (len(img->pixelsRed) != img->height * img->width) return false;
    }
    else {
        fprintf(stderr, "Invalid image type, got %d\n", img->type);
        return false;
    }

    return true;
}

tiffFile_t newFile(tiffImage_t* images, size_t imagesCount) {
    tiffFile_t file;

    file.byteOrder = TIFF_BIG_ENDIAN; // why this byte order? Because why not?

    file.images = images;
    file.imagesCount = imagesCount;

    return file;
}

void freeImage(tiffImage_t* image) {
    for (int i = 0; i < len(image->tags); ++i) {
        tiffDataTag_t tag = image->tags[i];
        if (tag.data != NULL) {
            free(tag.data);
        }
    }
    freeArray(image->tags);
}

void freeFile(tiffFile_t* file) {
    for (int i = 0; i < file->imagesCount; ++i) {
        freeImage(file->images + i);
    }
    free(file->images);
    file->imagesCount = 0;
}
