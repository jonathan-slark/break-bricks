#include <stdio.h>  // FILE, fopen, fprintf, stderr, perror, fseek, ftell, malloc, fclose, free
#include <stdlib.h> // size_t

#include "util.h"

// Constants
const char READ_ONLY_TEXT[]  = "r";
const char READ_ONLY_BIN[]   = "rb";
const char WRITE_ONLY_TEXT[] = "w";
const char WRITE_ONLY_BIN[]  = "wb";

// Have separate char and uint8_t loaders?
char* util_load(const char* file, const char* mode)
{
    FILE* fp = fopen(file, mode);
    if (!fp)
    {
	fprintf(stderr, "Could not open file %s\n", file);
	perror("fopen() error");
	return NULL;
    }

    if (fseek(fp, 0L, SEEK_END) != 0)
    {
	fprintf(stderr, "Error on seeking file %s\n", file);
	perror("fseek() error");
	return NULL;
    }
    long l = ftell(fp);
    if (l < 0)
    {
	fprintf(stderr, "Error on getting size of file %s\n", file);
	perror("ftell() error");
	return NULL;
    }
    size_t size = (size_t) l;
    rewind(fp);

    char* data = (char*) malloc((size + 1) * sizeof(char));
    if (fread(data, sizeof(char), size, fp) < size)
    {
	fprintf(stderr, "Error reading file %s\n", file);
	perror("fread() error");
	return NULL;
    }
    data[size] = '\0';

    if (fclose(fp) == EOF)
    {
	fprintf(stderr, "Error on closing file %s\n", file);
	perror("fclose() error");
    }

    return data;
}

void util_unload(char* data)
{
    free(data);
}
