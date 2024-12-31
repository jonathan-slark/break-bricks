/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

#define COUNT(x)           (sizeof x / sizeof x[0])
#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))
#define MAX(x, y)          ((x) > (y) ? (x) : (y))
#define MIN(x, y)          ((x) < (y) ? (x) : (y))

char* util_load(const char* filename);
void  util_unload(char* src);
