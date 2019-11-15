#ifndef TIFF_FORMAT_H_
#define TIFF_FORMAT_H_

/*
 * Tag ID names and ID numbers are sourced from:
 * https://www.fileformat.info/format/tiff/egff.htm
 */

// Tiff tag data types

typedef unsigned char BYTE; // -> BYTE
typedef char * ASCII; // null-terminated
typedef unsigned short SHORT; // -> WORD
typedef unsigned int LONG; // -> DWORD

typedef struct _RationalNum {
    LONG num;
    LONG denom;
} RATIONAL;

#define type_1 BYTE
#define type_2 ASCII
#define type_3 SHORT
#define type_4 LONG
#define type_5 RATIONAL

// TIFF 6.0 revision

typedef signed char SBYTE;
typedef unsigned char UNDEFINE;
typedef signed short SSHORT;
typedef signed int SLONG;

typedef struct _SignedRationalNum {
    SLONG num;
    SLONG denom;
} SRATIONAL;

typedef float FLOAT;
typedef double DOUBLE;

#define type_6 SBYTE
#define type_7 UNDEFINE
#define type_8 SSHORT
#define type_9 SLONG
#define type_10 SRATIONAL
#define type_11 FLOAT
#define type_12 DOUBLE

#define NUMBER_OF_TYPES 12

// index i should return sizeof type i
const unsigned long sizeOfTypeLookupTable[13] = { 0, 
    sizeof(type_1),
    sizeof(type_2),
    sizeof(type_3),
    sizeof(type_4),
    sizeof(type_5),
    sizeof(type_6),
    sizeof(type_7),
    sizeof(type_8),
    sizeof(type_9),
    sizeof(type_10),
    sizeof(type_11),
    sizeof(type_12),
};

/* === Classes === */

// Bi-level and Gray-scale
#define GrayScaleTags       { 254, 256, 257, 258, 259, 262, 273, 277, 278, 279, 282, 283, 296 }
#define GrayScaleTagCount 13

#define PaletteColorTags    { 254, 256, 257, 258, 259, 262, 273, 277, 278, 279, 282, 283, 296, 320 }
#define PaletteColorTagCount 14

#define RGBTags             { 254, 256, 257, 258, 259, 262, 273, 277, 278, 279, 282, 283, 296, 284 }
#define RGBTagCount 14

#define YCbCrTags           { 254, 256, 257, 258, 259, 262, 273, 277, 278, 279, 282, 283, 296, 529, 530, 531, 532 }
#define YCbCrTagCount 17

/* === Define Tag IDs === */

#define Artist 315
#define BitsPerSample 258
#define CellLength 265
#define CellWidth 264
#define ColorMap 320
#define ColorResponseCurve 301
#define ColorResponseUnit 300
#define Compression 259

/* === <Types for Compression> === */
#define Uncompressed 1
#define CCITT_1D 2
#define CCITT_Group_3 3
#define CCITT_Group_4 4
#define LZW 5
#define JPEG 6
#define Uncompressed 32771
#define Packbits 32773
/* === </Types for Compression> === */

#define Copyright 33432
#define DateTime 306
#define DocumentName 269
#define DotRange 336
#define ExtraSamples 338
#define FillOrder 266
#define FreeByteCounts 289
#define FreeOffsets 288
#define GrayResponseCurve 291
#define GrayResponseUnit 290
#define HalftoneHints 321
#define HostComputer 316
#define ImageDescription 270
#define ImageHeight 257
#define ImageWidth 256
#define InkNames 333
#define InkSet 332
#define JPEGACTTables 521
#define JPEGDCTTables 520
#define JPEGInterchangeFormat 513
#define JPEGInterchangeFormatLength 514
#define JPEGLosslessPredictors 517
#define JPEGPointTransforms 518
#define JPEGProc 512
#define JPEGRestartInterval 515
#define JPEGQTables 519
#define Make 271
#define MaxSampleValue 281
#define MinSampleValue 280
#define Model 272
#define NewSubFileType 254
#define NumberOfInks 334
#define Orientation 274
#define PageName 285
#define PageNumber 297
#define PhotometricInterpretation 262

/* === <Types for PhotometricInterpretation> === */
#define WhiteIsZero 0
#define BlackIsZero 1
#define RGB 2
#define RGB_Palette 3
#define Tranparency_Mask 4
#define CMYK 5
#define YCbCr 6
#define CIELab 8
/* === </Types for PhotometricInterpretation> === */

#define PlanarConfiguration 284
#define Predictor 317
#define PrimaryChromaticities 319
#define ReferenceBlackWhite 532
#define ResolutionUnit 296
#define RowsPerStrip 278
#define SampleFormat 339
#define SamplesPerPixel 277
#define SMaxSampleValue 341
#define SMinSampleValue 340
#define Software 305
#define StripByteCounts 279
#define StripOffsets 273
#define SubFileType 255
#define T4Options 292
#define T6Options 293
#define TargetPrinter 337
#define Thresholding 263
#define TileByteCounts 325
#define TileLength 323
#define TileOffsets 324
#define TileWidth 322
#define TransferFunction 301
#define TransferRange 342
#define XPosition 286
#define XResolution 282
#define YCbCrCoefficients 529
#define YCbCrPositioning 531
#define YCbCrSubSampling 530
#define YPosition 287
#define YResolution 283
#define WhitePoint 318

/* === Types for Tag IDs === */

#define Artist_t ASCII
#define BitsPerSample_t SHORT
#define CellLength_t SHORT
#define CellWidth_t SHORT
#define ColorMap_t SHORT
#define ColorResponseCurve_t SHORT
#define ColorResponseUnit_t SHORT
#define Compression_t SHORT
#define Copyright_t ASCII
#define DateTime_t ASCII
#define DocumentName_t ASCII
#define DotRange_t SHORT // or BYTE
#define ExtraSamples_t BYTE
#define FillOrder_t SHORT
#define FreeByteCounts_t LONG
#define FreeOffsets_t LONG
#define GrayResponseCurve_t SHORT
#define GrayResponseUnit_t SHORT
#define HalftoneHints_t SHORT
#define HostComputer_t ASCII
#define ImageDescription_t ASCII
#define ImageHeight_t LONG // or SHORT
#define ImageWidth_t LONG // or SHORT
#define InkNames_t ASCII
#define InkSet_t SHORT
#define JPEGACTTables_t LONG
#define JPEGDCTTables_t LONG
#define JPEGInterchangeFormat_t LONG
#define JPEGInterchangeFormatLength_t LONG
#define JPEGLosslessPredictors_t SHORT
#define JPEGPointTransforms_t SHORT
#define JPEGProc_t SHORT
#define JPEGRestartInterval_t SHORT
#define JPEGQTables_t LONG
#define Make_t ASCII
#define MaxSampleValue_t SHORT
#define MinSampleValue_t SHORT
#define Model_t ASCII
#define NewSubFileType_t LONG
#define NumberOfInks_t SHORT
#define Orientation_t SHORT
#define PageName_t ASCII
#define PageNumber_t SHORT
#define PhotometricInterpretation_t SHORT
#define PlanarConfiguration_t SHORT
#define Predictor_t SHORT
#define PrimaryChromaticities_t RATIONAL
#define ReferenceBlackWhite_t LONG
#define ResolutionUnit_t SHORT
#define RowsPerStrip_t SHORT or LONG
#define SampleFormat_t SHORT
#define SamplesPerPixel_t SHORT
#define SMaxSampleValue_t // Any
#define SMinSampleValue_t // Any
#define Software_t ASCII
#define StripByteCounts_t LONG // or SHORT
#define StripOffsets_t LONG // or SHORT
#define SubFileType_t SHORT
#define T4Options_t LONG
#define T6Options_t LONG
#define TargetPrinter_t ASCII
#define Thresholding_t SHORT
#define TileByteCounts_t LONG // or SHORT
#define TileLength_t LONG // or SHORT
#define TileOffsets_t LONG
#define TileWidth_t LONG // or SHORT
#define TransferFunction_t SHORT
#define TransferRange_t SHORT
#define XPosition_t RATIONAL
#define XResolution_t RATIONAL
#define YCbCrCoefficients_t RATIONAL
#define YCbCrPositioning_t SHORT
#define YCbCrSubSampling_t SHORT
#define YPosition_t RATIONAL
#define YResolution_t RATIONAL
#define WhitePoint_t RATIONAL

#endif
