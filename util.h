/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#define UNUSED(x) (void) (x)

static const char readonly[] = "r";

char *load(const char *filename);
void unload(char *src);
