#ifndef TIFF_IMAGE_H_
#define TIFF_IMAGE_H_

#include <stdlib.h>

#include "header.h"
#include "tiffTagStorage.h"
#include "tiffFormat.h"

#define GRAY_SCALE 1
#define RGB 2

size_t getTypeSizeOf(int typeId);

/* 
 * use this when making a new image as it initialises the fields. Can result in seg faults if this func isn't used for new images
 * I found that out the hard way
 */
tiffImage_t newImage(imgType iType);

/*
 * checks if an image has all the required tags
 */
bool isValidImage(tiffImage_t* img);

tiffFile_t newFile(tiffImage_t* images, size_t imagesCount);

void freeImage(tiffImage_t* image);

void freeFile(tiffFile_t* file);

#endif
