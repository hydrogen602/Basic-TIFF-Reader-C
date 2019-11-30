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
    expandingArrayData_t* ptr = malloc(maxLen * size + sizeof(arrayData_t) + sizeof(expandingArrayData_t));
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
    return __newExpandableArray(length, size, length);
}

size_t len(void* ptr) {
    arrayData_t* p = ptr;
    p--;
    if (p->identifier != EXPANDING_ARRAY && p->identifier != FIXED_ARRAY) {
        printErrMsg("Array invalid");
        fprintf(stderr, "identifier is %x\n", p->identifier);
        return 0;
    }
    return p->len;
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
    else if ((p - 1)->identifier == EXPANDING_ARRAY) {
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
}
//*/
