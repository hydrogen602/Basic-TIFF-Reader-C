#ifndef TIFF_FORMAT_H_
#define TIFF_FORMAT_H_

/*
 * Tag ID names and ID numbers are sourced from:
 * https://www.fileformat.info/format/tiff/egff.htm
 */

// Tiff tag data types

typedef uint8_t BYTE; // -> BYTE
typedef uint8_t ASCII; // null-terminated
typedef uint16_t SHORT; // -> WORD
typedef uint32_t LONG; // -> DWORD

typedef struct _RationalNum {
    LONG num;
    LONG denom;
} RATIONAL;

#define BYTE_TypeID 1
#define ASCII_TypeID 2
#define SHORT_TypeID 3
#define LONG_TypeID 4
#define RATIONAL_TypeID 5

// TIFF 6.0 revision

typedef int8_t SBYTE;
typedef uint8_t UNDEFINE;
typedef int16_t SSHORT;
typedef int32_t SLONG;

typedef struct _SignedRationalNum {
    SLONG num;
    SLONG denom;
} SRATIONAL;

typedef float FLOAT;
typedef double DOUBLE;

#define SBYTE_TypeID 6
#define UNDEFINE_TypeID 7
#define SSHORT_TypeID 8
#define SLONG_TypeID 9
#define SRATIONAL_TypeID 10
#define FLOAT_TypeID 11
#define DOUBLE_TypeID 12

#define NUMBER_OF_TYPES 12

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

/* === Classes === */

// Bi-level and Gray-scale
const int GRAY_SCALE_TAGS[] = { 254, 256, 257, 258, 259, 262, 273, 277, 278, 279, 282, 283, 296 };
#define GRAY_SCALE_TAGS_COUNT 13

const int PALETTE_COLOR_TAGS[] = { 254, 256, 257, 258, 259, 262, 273, 277, 278, 279, 282, 283, 296, 320 };
#define PALETTE_COLOR_TAGS_COUNT 14

const int RGB_TAGS[] = { 254, 256, 257, 258, 259, 262, 273, 277, 278, 279, 282, 283, 296, 284 };
#define RGB_TAGS_COUNT 14

const int YCbCr_TAGS = { 254, 256, 257, 258, 259, 262, 273, 277, 278, 279, 282, 283, 296, 529, 530, 531, 532 };
#define YCbCr_TAGS_COUNT 17

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
#define C_Uncompressed 1
#define C_CCITT_1D 2
#define C_CCITT_Group_3 3
#define C_CCITT_Group_4 4
#define C_LZW 5
#define C_JPEG 6
//#define C_Uncompressed 32771 // obsolete in later versions of tiff
#define C_Packbits 32773
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
#define PI_WhiteIsZero 0
#define PI_BlackIsZero 1
#define PI_RGB 2
#define PI_RGB_Palette 3
#define PI_Tranparency_Mask 4
#define PI_CMYK 5
#define PI_YCbCr 6
#define PI_CIELab 8
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
