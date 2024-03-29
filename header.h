#ifndef HEADER_H_
#define HEADER_H_
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include "tiffFormat.h"

// if f is NULL, print msg to stderr and return -1
#define assertNotNull(f, msg) if ((f) == NULL) { fprintf(stderr, (msg)); return -1; }
#define assertNotNullEx(f, msg, var) if ((f) == NULL) { fprintf(stderr, (msg), (var)); return -1; }

#define assertNotNullPtrExc(v) if ((v) == NULL) { fprintf(stderr, "NullPointerException at %d in file %s\n", __LINE__, __FILE__); }

// if bool is not evaluated to true, print msg to stderr and return -1
#define assert(bool, msg) if (!(bool)) { fprintf(stderr, (msg)); return -1; }
#define assertEx(bool, msg, var) if (!(bool)) { fprintf(stderr, (msg), (var)); return -1; }

// like before but runs a cleanup command 
#define assertCmd(bool, msg, cmd) if (!(bool)) { fprintf(stderr, (msg)); cmd; return -1; }

#define printErrMsg(error) fprintf(stderr, "%s at %d in file %s\n", error, __LINE__, __FILE__)


#endif
