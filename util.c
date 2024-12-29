#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "util.h"

char *load(const char *filename)
{
    FILE *fp = fopen(filename, readonly);
    if (!fp)
        term(EXIT_FAILURE, "Could not open file %s.\n", filename);

    if (fseek(fp, 0L, SEEK_END) != 0)
        term(EXIT_FAILURE, "Error on seeking file %s.\n", filename);
    long l = ftell(fp);
    if (l < 0)
        term(EXIT_FAILURE, "Error on getting size of file %s.\n", filename);
    size_t size = (size_t)l;
    rewind(fp);

    char *src = (char *)malloc((size + 1) * sizeof(char));
    if (fread(src, sizeof(char), size, fp) < size)
        term(EXIT_FAILURE, "Error reading file %s.\n", filename);
    src[size] = '\0';

    if (fclose(fp) == EOF)
        term(EXIT_FAILURE, "Error on closing file %s.\n", filename);

    return src;
}

void unload(char *src)
{
    free(src);
}
