#include <stdlib.h>
#include "array.h"
#include "string.h"
#include "../header.h"

/*
 *  /------------------------- allocated mem -------------------------\
 * [ data structs ][ 0 ][ 1 ][ 2 ][ 3 ][ 4 ]...[ n ]
 *                   ^
 *                  ptr
 * maxLen stores the maximum amount of data that can be stored in the array,
 * which is the size of the allocated memory minus 2 * sizeof(size_t)
 * len stores how much of the array is actually used
 * If the array is of fixed size, maxLen and len should equal
 * If the array is intended to expand, maxLen > len
 */

void* newArray(size_t length, size_t size) {
    arrayData_t* ptr = malloc(length * size + sizeof(arrayData_t));
    ptr->identifier = FIXED_ARRAY;
    ptr->len = length;
    return (char *)ptr + sizeof(arrayData_t);
}

void* __newExpandableArray(size_t length, size_t size, size_t maxLen) {
    // [expandingArrayData_t][arrayData_t] array
    unsigned char* tmp = malloc(maxLen * size + sizeof(arrayData_t) + sizeof(expandingArrayData_t));
    for (size_t t = 0; t < maxLen * size + sizeof(arrayData_t) + sizeof(expandingArrayData_t); ++t) {
        tmp[t] = 0;
    }
    expandingArrayData_t* ptr = (expandingArrayData_t*)tmp;
    ptr->elementSize = size;
    ptr->maxLen = maxLen;
    arrayData_t* ptr_tmp = (arrayData_t*) (ptr + 1);
    ptr_tmp->identifier = EXPANDING_ARRAY;
    ptr_tmp->len = length;

    char* data = (char *) (ptr_tmp + 1);
    for (size_t i = 0; i < length; ++i) {
        data[i] = 0; // initialize with zero
    }
    return data;
}

void* newExpandableArray(size_t length, size_t size) {
    size_t sz = (length < 16) ? 16 : length;
    return __newExpandableArray(length, size, sz);
}

byte* __newBuffer(size_t length, size_t maxLen) {
    byte* ptr = __newExpandableArray(length, 1, maxLen);

    arrayData_t* p = ptr;
    p--;
    p->identifier = BUFFER_ARRAY;

    return ptr;
}

byte* newBuffer(size_t length) {
    size_t sz = (length < 16) ? 16 : length;
    if (sizeof(byte) != 1) {
        printErrMsg("unsigned char needs to be 1 byte long for this code");
        exit(EXIT_FAILURE);
    }

    return __newBuffer(length, sz);
}

size_t len(void* ptr) {
    arrayData_t* p = ptr;
    p--;
    if (p->identifier != EXPANDING_ARRAY && p->identifier != FIXED_ARRAY && p->identifier != BUFFER_ARRAY) {
        printErrMsg("Array invalid");
        fprintf(stderr, "identifier is %x\n", p->identifier);
        return 0;
    }
    return p->len;
}

unsigned char arrayType(void* ptr) {
    arrayData_t* p = ptr;
    p--;
    if (p->identifier != EXPANDING_ARRAY && p->identifier != FIXED_ARRAY && p->identifier != BUFFER_ARRAY) {
        printErrMsg("Array invalid");
        fprintf(stderr, "identifier is %x\n", p->identifier);
        return 0;
    }
    return p->identifier;
}

// checks if the index requires expanding the array
void* checkIndex(void* ptr, size_t index) {
    arrayData_t* p = ptr;
    p--;
    if (p->identifier == EXPANDING_ARRAY) {
        if (index < len(ptr)) {
            // nothing to do
        }
        else {
            // expand array!
            expandingArrayData_t* ead = (expandingArrayData_t*) p;
            ead--;
            if (index < ead->maxLen) {
                // enough space still there
                p->len++;
            }
            else {
                // expand memory block
                printf("Expanding to %lu\n", ead->maxLen * 2);
                printf("index = %ld\n", index);
                void* ptrNew = __newExpandableArray(p->len + 1, ead->elementSize, ead->maxLen * 2);
                memcpy(ptrNew, ptr, p->len * ead->elementSize);
                freeArray(ptr);
                return ptrNew;
            }
        }
    }
    else if (p->identifier == FIXED_ARRAY) {
        printErrMsg("This is a fixed array, checkIndex should only be called on expanding arrays");
    }
    else if (p->identifier == BUFFER_ARRAY) {
        printErrMsg("This is a buffer, checkIndex should only be called on expanding arrays");
    }
    else {
        printErrMsg("Array Invalid");
    }

    return ptr;
}

byte* checkBufferIndex(byte* ptr, size_t offset, size_t elementSize) {
    arrayData_t* p = ptr;
    p--;
    if (p->identifier == BUFFER_ARRAY) {
        if (offset + elementSize < len(ptr)) {
            // nothing to do
        }
        else {
            // expand array!
            expandingArrayData_t* ead = (expandingArrayData_t*) p;
            ead--;
            if (offset + elementSize < ead->maxLen) {
                // enough space still there
                p->len += elementSize;
            }
            else {
                // expand memory block
                size_t newSize = ead->maxLen;
                do {
                    newSize *= 2;
                } while(newSize <= offset + elementSize);
                
                printf("Expanding to %lu\n", newSize);
                printf("offset = %ld\n", offset);
                void* ptrNew = __newExpandableArray(p->len + 1, ead->elementSize, ead->maxLen * 2);
                // change ptr
                memcpy(ptrNew, ptr, p->len * ead->elementSize);
                freeArray(ptr);
                return ptrNew;
            }
        }
    }
    else if (p->identifier == FIXED_ARRAY) {
        printErrMsg("This is a fixed array, checkBufferIndex should only be called on buffers");
    }
    else if (p->identifier == EXPANDING_ARRAY) {
        printErrMsg("This is an expanding array, checkBufferIndex should only be called on buffers");
    }
    else {
        printErrMsg("Array Invalid");
    }

    return ptr;
}

void freeArray(void* ptr) {
    arrayData_t* p = ptr;
    if ((p - 1)->identifier == FIXED_ARRAY) {
        free(p - 1);
    }
    else if ((p - 1)->identifier == EXPANDING_ARRAY || (p - 1)->identifier == BUFFER_ARRAY) {
        free((char*)p - sizeof(arrayData_t) - sizeof(expandingArrayData_t));
    }
    else {
        printErrMsg("Array Invalid");
    }
}

/*
 * Test code for fixed array, expanding array
 * Check with valgrind to ensure that there are no memory issues
 */
#ifdef TESTING
int main(void) {
    // expandable array
    printf("basic expandable array test\n");
    int* arr = newExpandableArray(10, sizeof(int));

    for (int i = 0; i < len(arr); i++) {
        arr[i] = i * i;
    }

    for (int i = 0; i < len(arr); i++) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }

    freeArray(arr);

    // fixed array
    printf("basic fixed array test\n");
    arr = newArray(10, sizeof(int));

    for (int i = 0; i < len(arr); i++) {
        arr[i] = i * i;
    }

    for (int i = 0; i < len(arr); i++) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }

    freeArray(arr);

    printf("expandable array test\n");
    // expanding array with expanding
    arr = newExpandableArray(10, sizeof(int));

    for (int i = 0; i < 30; i++) {
        setIndex(arr, i, i * i);
        //arr = checkIndex(arr, i);
        //arr[i] = i * i;
    }

    for (int i = 0; i < len(arr); i++) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }

    freeArray(arr);

    printf("more expanding array testing\n");
    // append macro
    arr = newExpandableArray(0, sizeof(int));

    for (int i = 0; i < 30; i++) {
        append(arr, i * i);
        //arr = checkIndex(arr, i);
        //arr[i] = i * i;
    }

    for (int i = 0; i < len(arr); i++) {
        printf("arr[%d] = %d\n", i, arr[i]);
    }

    freeArray(arr);

}
#endif
