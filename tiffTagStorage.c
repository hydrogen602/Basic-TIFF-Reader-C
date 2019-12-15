#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tiffTagStorage.h"
#include "tiffFormat.h"
#include "tiffImage.h"
#include "support/array.h"

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

tiffDataTag_t* findTag(WORD tagId, tiffDataTag_t* tags) {
    for (int i = 0; i < len(tags); ++i) {
        if ((tags + i)->tagId == tagId) return tags + i;
    }
    return NULL;
}

void freeDataTag(tiffDataTag_t *t) {
    free(t->data);
}

void tagPrintDebug(tiffDataTag_t *t) {
    unsigned int n1 = 0;
    
    switch (t->tagId)
    {
        case 315: printf("Artist\n"); break;
        case 258: printf("BitsPerSample\n"); break;
        case 265: printf("CellLength\n"); break;
        case 264: printf("CellWidth\n"); break;
        case 320: printf("ColorMap\n"); break;
        case 301: printf("ColorResponseCurve\n"); break;
        case 300: printf("ColorResponseUnit\n"); break;
        case 259: 
            printf("Compression: ");
            short n;
            memcpy(&n, t->data, sizeof(short));

            switch (n)
            {
                case 1: printf("Uncompressed\n"); break;
                case 2: printf("CCITT_1D\n"); break;
                case 3: printf("CCITT_Group_3\n"); break;
                case 4: printf("CCITT_Group_4\n"); break;
                case 5: printf("LZW\n"); break;
                case 6: printf("JPEG\n"); break;
            
                default: printf("unknown: %d\n", t->data[0]); break;
            }
            break;
        case 32771: printf("Uncompressed\n"); break;
        case 32773: printf("Packbits\n"); break;
        case 33432: printf("Copyright\n"); break;
        case 306: printf("DateTime\n"); break;
        case 269: printf("DocumentName\n"); break;
        case 336: printf("DotRange\n"); break;
        case 338: printf("ExtraSamples\n"); break;
        case 266: printf("FillOrder\n"); break;
        case 289: printf("FreeByteCounts\n"); break;
        case 288: printf("FreeOffsets\n"); break;
        case 291: printf("GrayResponseCurve\n"); break;
        case 290: printf("GrayResponseUnit\n"); break;
        case 321: printf("HalftoneHints\n"); break;
        case 316: printf("HostComputer\n"); break;
        case 270: printf("ImageDescription\n"); break;
        case 257: 
            printf("ImageHeight: "); 

            memcpy(&n1, t->data, sizeof(n1));
            printf("%u\n", n1);
            break;
        case 256: 
            printf("ImageWidth: ");

            memcpy(&n1, t->data, sizeof(n1));
            printf("%u\n", n1);
            break;
        case 333: printf("InkNames\n"); break;
        case 332: printf("InkSet\n"); break;
        case 521: printf("JPEGACTTables\n"); break;
        case 520: printf("JPEGDCTTables\n"); break;
        case 513: printf("JPEGInterchangeFormat\n"); break;
        case 514: printf("JPEGInterchangeFormatLength\n"); break;
        case 517: printf("JPEGLosslessPredictors\n"); break;
        case 518: printf("JPEGPointTransforms\n"); break;
        case 512: printf("JPEGProc\n"); break;
        case 515: printf("JPEGRestartInterval\n"); break;
        case 519: printf("JPEGQTables\n"); break;
        case 271: printf("Make\n"); break;
        case 281: printf("MaxSampleValue\n"); break;
        case 280: printf("MinSampleValue\n"); break;
        case 272: printf("Model\n"); break;
        case 254: printf("NewSubFileType\n"); break;
        case 334: printf("NumberOfInks\n"); break;
        case 274: printf("Orientation\n"); break;
        case 285: printf("PageName\n"); break;
        case 297: printf("PageNumber\n"); break;
        case 262:
            printf("PhotometricInterpretation: ");
            short n2;
            memcpy(&n2, t->data, sizeof(short));

            switch (n2)
            {
                case 0: printf("WhiteIsZero\n"); break;
                case 1: printf("BlackIsZero\n"); break;
                case 2: printf("RGB\n"); break;
                case 3: printf("RGB Palette\n"); break;
                case 4: printf("Tranparency Mask\n"); break;
                case 5: printf("CMYK\n"); break;
                case 6: printf("YCbCr\n"); break;
                case 8: printf("CIELab\n"); break;
            
                default: printf("unknown: %d\n", n2); break;
            }
            break;
        case 284: 
            printf("PlanarConfiguration: ");

            memcpy(&n1, t->data, sizeof(n1));
            printf("%u\n", n1);
            break;
        case 317: printf("Predictor\n"); break;
        case 319: printf("PrimaryChromaticities\n"); break;
        case 532: printf("ReferenceBlackWhite\n"); break;
        case 296: printf("ResolutionUnit\n"); break;
        case 278: 
            printf("RowsPerStrip: ");

            memcpy(&n1, t->data, sizeof(n1));
            printf("%u\n", n1);
            break;
        case 339: printf("SampleFormat\n"); break;
        case 277: printf("SamplesPerPixel\n"); break;
        case 341: printf("SMaxSampleValue\n"); break;
        case 340: printf("SMinSampleValue\n"); break;
        case 305: printf("Software\n"); break;
        case 279: printf("StripByteCounts\n"); break;
        case 273: printf("StripOffsets\n"); break;
        case 255: printf("SubFileType\n"); break;
        case 292: printf("T4Options[2]\n"); break;
        case 293: printf("T6Options[3]\n"); break;
        case 337: printf("TargetPrinter\n"); break;
        case 263: printf("Thresholding\n"); break;
        case 325: printf("TileByteCounts\n"); break;
        case 323: printf("TileLength\n"); break;
        case 324: printf("TileOffsets\n"); break;
        case 322: printf("TileWidth\n"); break;
        //case 301: printf("TransferFunction[4]\n"); break;
        case 342: printf("TransferRange\n"); break;
        case 286: printf("XPosition\n"); break;
        case 282: printf("XResolution\n"); break;
        case 529: printf("YCbCrCoefficients\n"); break;
        case 531: printf("YCbCrPositioning\n"); break;
        case 530: printf("YCbCrSubSampling\n"); break;
        case 287: printf("YPosition\n"); break;
        case 283: printf("YResolution\n"); break;
        case 318: printf("WhitePoint\n"); break;
    
    default:
        printf("Unknown tag: id=%d\n", t->tagId);
        break;
    }
}

// int main(void) {
//     printf("test\n");
//     tiffDataTag_t dt = dataTag(1, 1, 1);

//     freeDataTag(dt);
// }
