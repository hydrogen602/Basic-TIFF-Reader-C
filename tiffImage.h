#ifndef TIFF_IMAGE_H_
#define TIFF_IMAGE_H_

#include <stdlib.h>

#include "header.h"
#include "tiffTagStorage.h"
#include "tiffFormat.h"

typedef unsigned char pixel_t;

typedef struct _GrayScaleTiffImage {
    tiffHead_t header;
    tiffIFDTag_t* ifds;
    size_t ifdsCount;

    pixel_t* pixels;
    size_t pixelCount;
} tiffImageGS_t;

typedef struct _ColorTiffImage {
    tiffHead_t header;
    tiffIFDTag_t* ifds;
    size_t ifdsCount;

    pixel_t* pixelsRed;
    pixel_t* pixelsGreen;
    pixel_t* pixelsBlue;

    size_t pixelCount;
} tiffImageRGB_t;

#endif