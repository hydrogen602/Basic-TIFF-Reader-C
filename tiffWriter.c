#include "header.h"
#include "tiffTagStorage.h"
#include "tiffImage.h"
#include "support/array.h"
// todo

int ceilDivision(int a, int b) {
    // f = lambda total, rows: (total // rows) + (1 if total % rows > 0 else 0)

    return (a / b) + ((a % b > 0) ? 1 : 0);
}

int createWitdhHeightTag(tiffImage_t* t) {
    tiffDataTag_t* hTag = findTag(ImageHeight, t->tags);
    if (hTag != NULL) {
        // nothing to do
    }
    else {
        tiffDataTag_t tag = newDataTag(ImageHeight, LONG_TypeID, 1);
        set(t->height, &tag, 0);

        // add to tags
        append(t->tags, tag);
    }

    tiffDataTag_t* wTag = findTag(ImageWidth, t->tags);
    if (wTag != NULL) {
        // nothing to do
    }
    else {
        tiffDataTag_t tag = newDataTag(ImageWidth, LONG_TypeID, 1);
        set(t->width, &tag, 0);

        // add to tags
        append(t->tags, tag);
    }

    return 0;
}

int createRowsPerStripTag(tiffImage_t* t) {
    tiffDataTag_t* test = findTag(RowsPerStrip, t->tags);
    if (test != NULL) {
        // already known
        return 0;
    }

    uint64_t size = t->width * t->height;

    const int pixelsPerRow = 40000;

    int strips = size / pixelsPerRow;

    int rowsPerStrip = ceilDivision(t->height, strips);
    
    printf("Strips = %d\n", strips);
    printf("RowsPerStrip = %d\n", rowsPerStrip);

    tiffDataTag_t tag = newDataTag(RowsPerStrip, LONG_TypeID, 1);

    set(rowsPerStrip, &tag, 0);

    // add to tags
    append(t->tags, tag);

    return 0;
}

int createMissingTags(tiffImage_t* t) {
    if (t->height == 0 || t->width == 0) {
        printErrMsg("Bad height or width of image");
        return -1;
    }

    int r;
    r = createWitdhHeightTag(t);
    if (r != 0) { return r; }
    r = createRowsPerStripTag(t);
    if (r != 0) { return r; }

    return 0;
}

int _tiffWriter_openFileAndWrite(const char* filename, unsigned char* buffer) {
    FILE* f = fopen(filename, "wb");

    if (f == NULL) {
        printErrMsg("Cannot open file");
    }

    for (size_t i = 0; i < len(buffer); ++i) {
        if (fputc(buffer[i], f) == EOF) {
            printErrMsg("write failed");
            return -1;
        }
    }

    fclose(f);

    return 0;
}

int main(void) {
    tiffImage_t img = makeImage(RGB);
    img.height = 100;
    img.width = 100;

    int r = createMissingTags(&img);
    if (r != 0) {
        printErrMsg("createMissingTags failed");
    }

    if (!isValidImage(&img)) {
        printErrMsg("Not valid image, cannot write to file\n");
        return -1;
    }

    unsigned char* buffer = newExpandableArray(img.height * img.width * 3, sizeof(unsigned char));
    if (buffer == NULL) {
        printErrMsg("Malloc failed");
        return -1;
    }

    

    
}
