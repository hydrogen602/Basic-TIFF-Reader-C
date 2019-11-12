
// https://www.fileformat.info/format/tiff/egff.htm

typedef unsigned char BYTE; // -> BYTE
typedef char * ASCII; // null-terminated
typedef unsigned short SHORT; // -> WORD
typedef unsigned int LONG; // -> DWORD

typedef struct _RationalNum {
    LONG num;
    LONG denom;
} RATIONAL;

/*
Req for Gray-scale
254 NewSubfileType
256 ImageWidth
257 ImageLength
258 BitsPerSample
259 Compression
262 PhotometricInterpretation
273 StripOffsets
277 SamplesPerPixel
278 RowsPerStrip
279 StripByteCounts
282 XResolution
283 YResolution
296 ResolutionUnit

RGB
Above +
284 PlanarConfiguration
*/
