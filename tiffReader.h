
#ifndef tiffReader_H_
#define tiffReader_H_

typedef unsigned short WORD; // 2 bytes
typedef unsigned int DWORD; // 4 bytes

// https://www.fileformat.info/format/tiff/egff.htm
typedef struct _TifTag
{
	WORD    TagId;       /* The tag identifier  */
	WORD    DataType;    /* The scalar type of the data items  */
	DWORD   DataCount;   /* The number of items in the tag data  */
	DWORD   DataOffset;  /* The byte offset to the data items  */
} TIFTAG;

typedef struct _TifIfd
{
	WORD    NumDirEntries;    /* Number of Tags in IFD  */
	TIFTAG* TagList;        /* Array of Tags  */
	DWORD   NextIFDOffset;    /* Offset to next IFD  */

    // main tiff tags
    WORD    height;
    WORD    width;
    WORD*   bitsPerSample;
    WORD    compression;
    WORD    photometricInterpretation;
    DWORD*  stripOffsets;
    DWORD*  stripByteCounts;
    WORD    stripCount;
    WORD    samplesPerPixel;
    DWORD   rowsPerStrip;

    DWORD   xResolutionNum;
    DWORD   xResolutionDen;
    DWORD   yResolutionNum;
    DWORD   yResolutionDen;



} TIFIFD;

typedef struct _TiffHeader
{
	WORD    identifier;  /* Byte-order Identifier */
	WORD    version;     /* TIFF version number (always 2Ah) */
	DWORD   IFDOffset;   /* Offset of the first Image File Directory */
    TIFIFD  nextIDF;     /* Not in image, but to make a linked list data struct in c */
} TIFHEAD;


boolean isProperHeader(TIFHEAD* t);

int sizeofFile(const char* filename);

int fileReader(const char* filename, unsigned char* buffer, unsigned int fileSize);

short readShortFromBuffer(boolean littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

int readIntFromBuffer(boolean littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

int parseHeader(TIFHEAD* t, unsigned char* buffer, unsigned int fileSize);

int parseIFD(TIFIFD* t, boolean littleEndian, unsigned int offset, unsigned char* buffer, unsigned int fileSize);

#endif
