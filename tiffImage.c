#include <stdlib.h>
#include <stdio.h>

#include "header.h"
#include "tiffImage.h"
#include "tiffTagStorage.h"
#include "tiffFormat.h"

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

bool isValidImage(tiffImage_t* img) {
    // validating gray scale tags are there
    int* requiredTags = NULL;
    size_t requiredTagLength = 0;

    if (img->type == GRAY_SCALE) {
        requiredTags = grayScaleTags;
        requiredTagLength = GRAY_SCALE_TAGS_COUNT;

        if (img->pixels == NULL) return false;
        if (img->pixelsBlue != NULL) return false;
        if (img->pixelsGreen != NULL) return false;
        if (img->pixelsRed != NULL) return false;
    }
    else if (img->type == RGB) {
        requiredTags = rgbTags;
        requiredTagLength = RGB_TAGS_COUNT;

        if (img->pixels != NULL) return false;
        if (img->pixelsBlue == NULL) return false;
        if (img->pixelsGreen == NULL) return false;
        if (img->pixelsRed == NULL) return false;
    }
    else {
        fprintf(stderr, "Invalid image type, got %d\n", img->type);
        return false;
    }

    for (unsigned long t = 0; t < requiredTagLength; ++t) {
        bool found = false;
        for (unsigned long i = 0; i < img->tagCount; ++i) {
            if ((img->tags + i)->tagId == requiredTags[t]) {
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

tiffFile_t makeFile(tiffImage_t* images, size_t imagesCount) {
    tiffFile_t file;

    tiffHead_t head;
    head.identifier = 0x4d4d; // why this byte order? Because why not?
    head.version = 0x002a;
    head.IFDOffset = 0; // set later

    file.header = head;

    file.images = images;
    file.imagesCount = imagesCount;

    return file;
}


