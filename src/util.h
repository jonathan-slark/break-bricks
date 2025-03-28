#pragma once

// Macros
#define COUNT(x)           (sizeof x / sizeof x[0])
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define MAX(x, y)          ((x) > (y) ? (x) : (y))
#define MIN(x, y)          ((x) < (y) ? (x) : (y))

// Constants
extern const char READ_ONLY_TEXT[];
extern const char READ_ONLY_BIN[];
extern const char WRITE_ONLY_TEXT[];
extern const char WRITE_ONLY_BIN[];

// Function prototypes
char* util_load(const char* file, const char* mode);
void  util_unload(char* data);
