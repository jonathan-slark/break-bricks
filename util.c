#include <cglm/struct.h>
#include <stdio.h>
#include <stdlib.h>

#include "util.h"

// Have separate char and uint8_t loaders?
char* util_load(const char* file, const char* mode) {
    FILE* fp = fopen(file, mode);
    if (!fp) {
	fprintf(stderr, "Could not open file %s.\n", file);
	return NULL;
    }

    if (fseek(fp, 0L, SEEK_END) != 0) {
	fprintf(stderr, "Error on seeking file %s.\n", file);
	return NULL;
    }
    long l = ftell(fp);
    if (l < 0) {
	fprintf(stderr, "Error on getting size of file %s.\n", file);
	return NULL;
    }
    size_t size = (size_t) l;
    rewind(fp);

    char* data = (char*) malloc((size + 1) * sizeof(char));
    if (fread(data, sizeof(char), size, fp) < size) {
	fprintf(stderr, "Error reading file %s.\n", file);
	return NULL;
    }
    data[size] = '\0';

    if (fclose(fp) == EOF) {
	fprintf(stderr, "Error on closing file %s.\n", file);
	return NULL;
    }

    return data;
}

void util_unload(char* data) {
    free(data);
}
