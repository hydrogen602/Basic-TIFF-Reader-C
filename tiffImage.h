#ifndef TIFF_IMAGE_H_
#define TIFF_IMAGE_H_

#include <stdlib.h>

#include "header.h"
#include "tiffTagStorage.h"
#include "tiffFormat.h"

typedef unsigned char pixel_t;

const int grayScaleTags[] = GRAY_SCALE_TAGS;
const int rgbTags[] = RGB_TAGS;

typedef struct _GenericTiffImage {
    tiffHead_t header;
    tiffIFDTag_t* ifds;
    size_t ifdsCount;

    pixel_t* pixels;

    pixel_t* pixelsRed;
    pixel_t* pixelsGreen;
    pixel_t* pixelsBlue;

    size_t pixelCount;

    int* requiredTags;
} tiffImage_t;

#endif