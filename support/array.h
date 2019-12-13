#ifndef ArrayWithLength_H_
#define ArrayWithLength_H_

#include <stddef.h>

#define FIXED_ARRAY 0xfa
#define EXPANDING_ARRAY 0xea
#define BUFFER_ARRAY 0xba

typedef unsigned char byte;

/*
 * runs checkIndex to expand an expandable array if necessary, then sets the index of the array to the given value
 * 
 * - arr should be a expandable array
 * - index should be 0 <= index <= len(array)  <- index can equal length because checkIndex expands the array size in that case
 * - value is the value the index of the array should be set to
 */
#define setIndex(arr, index, value) (arr) = checkIndex((arr), (index)); *((arr) + (index)) = (value);
#define append(arr, value) (arr) = checkIndex((arr), len(arr)); *((arr) + len(arr) - 1) = (value);

typedef struct _BasicArrayData {
    size_t len;
    unsigned char identifier;
} arrayData_t;

typedef struct _ExpandingArrayData {
    size_t maxLen;
    size_t elementSize;
} expandingArrayData_t;

/*
 * make a new array of fixed size
 * - length is num of elements
 * - size is sizeof(element) 
 * returns a void ptr that can be indexed like a regular array, i.e. arr[i] = 5;
 */
void* newArray(size_t length, size_t size);

/*
 * make a new array of variable size
 * - length is num of elements
 * - size is sizeof(element) 
 * returns a void ptr
 * 
 * NOTE:
 *  It can be indexed like a regular array, i.e. arr[i] = 5;
 *  Before setting an index, run ptr = checkIndex(ptr, index); first!
 *  
 * The setIndex macro combines checkIndex with setting the array index for easier use
 */
void* newExpandableArray(size_t length, size_t size);

/*
 * make a new array of variable size and type byte, aka unsigned char
 * this is useful for reading and writing binary file formats where
 * the data in those bytes could be of differing sizes and types
 * 
 * NOTE:
 *  It can be indexed like a regular array, i.e. arr[i] = 5;
 *  Before setting an index, run ptr = checkBufferIndex(ptr, index, elementSize); first!
 *  
 */
byte* newBuffer(size_t length);

/*
 * returns the length of an array
 * - ptr is either a fixed or variable array
 */
size_t len(void* ptr);

/*
 * returns the type of an array
 * - result is one of the three macros: FIXED_ARRAY, EXPANDING_ARRAY, or BUFFER_ARRAY
 */
unsigned char arrayType(void* ptr);

/*
 * check if an index for an expandable array requires expanding the size of the array
 * should be called before whenever an array element is being set
 * - ptr is an expandable array
 * - index is the index of the array that is about to be set
 *      - NOTE: index should be only be one more than the length: 0 <= index <= len
 * usage:
 *  arr = checkIndex(arr, index);
 * 
 * The setIndex macro combines checkIndex with setting the array index for easier use
 */
void* checkIndex(void* ptr, size_t index);

/*
 * check if an index for a buffer requires expanding the size of the array
 * should be called before whenever bytes are written to the buffer
 * - ptr is an expandable array
 * - offset is where the write to the buffer starts
 *          - NOTE: offset should be greater or equal to zero
 * - elementSize is the number of bytes being written
 *      
 * usage:
 *  arr = checkBufferIndex(arr, offset, sizeof(type));
 * 
 */
byte* checkBufferIndex(byte* ptr, size_t offset, size_t elementSize);

/*
 * frees the memory allocated for an array
 * - ptr should be an expanding or fixed array
 */
void freeArray(void* ptr);

#endif
