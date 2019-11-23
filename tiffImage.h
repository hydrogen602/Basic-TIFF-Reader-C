#ifndef TIFF_IMAGE_H_
#define TIFF_IMAGE_H_

#include <stdlib.h>

#include "header.h"
#include "tiffTagStorage.h"
#include "tiffFormat.h"

#define GRAY_SCALE 1
#define RGB 2

typedef int imgType;

typedef unsigned char pixel_t;

// represents a tiff image. One tiff file can contain multiple images
typedef struct _GenericTiffSubFile { 
    DWORD nextIFDOffset; // Offset to next IFD

    size_t tagCount;    // length of tag array
    tiffDataTag_t* tags; // tags

    pixel_t* pixels; // gray scale
    // OR
    pixel_t* pixelsRed; // rgb
    pixel_t* pixelsGreen;
    pixel_t* pixelsBlue;

    size_t pixelCount;

    imgType type; // GRAY_SCALE or RGB

} tiffImage_t;

// represents a tiff file
typedef struct _TiffFile { 
    tiffHead_t header;
    tiffImage_t* images;
    size_t imagesCount;
} tiffFile_t;

#endif