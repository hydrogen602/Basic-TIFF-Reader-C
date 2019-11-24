#ifndef HEADER_H_
#define HEADER_H_
#include <stdbool.h>
#include <stdint.h>
#include "tiffFormat.h"

// if f is NULL, print msg to stderr and return -1
#define assertNotNull(f, msg) if ((f) == NULL) { fprintf(stderr, (msg)); return -1; }
#define assertNotNullEx(f, msg, var) if ((f) == NULL) { fprintf(stderr, (msg), (var)); return -1; }

// if bool is not evaluated to true, print msg to stderr and return -1
#define assert(bool, msg) if (!(bool)) { fprintf(stderr, (msg)); return -1; }
#define assertEx(bool, msg, var) if (!(bool)) { fprintf(stderr, (msg), (var)); return -1; }

// like before but runs a cleanup command 
#define assertCmd(bool, msg, cmd) if (!(bool)) { fprintf(stderr, (msg)); cmd; return -1; }

#define printError(msg) fprintf(stderr, (msg))

#endif
