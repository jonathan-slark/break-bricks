#include <cglm/struct.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "util.h"

// Have separate char and uint8_t loaders?
char* util_load(const char* file, const char* mode) {
    FILE* fp = fopen(file, mode);
    if (!fp) {
	main_term(EXIT_FAILURE, "Could not open file %s.\n", file);
    }

    if (fseek(fp, 0L, SEEK_END) != 0) {
	main_term(EXIT_FAILURE, "Error on seeking file %s.\n", file);
    }
    long l = ftell(fp);
    if (l < 0) {
	main_term(EXIT_FAILURE, "Error on getting size of file %s.\n", file);
    }
    size_t size = (size_t) l;
    rewind(fp);

    char* src = (char*) malloc((size + 1) * sizeof(char));
    if (fread(src, sizeof(char), size, fp) < size) {
	main_term(EXIT_FAILURE, "Error reading file %s.\n", file);
    }
    src[size] = '\0';

    if (fclose(fp) == EOF) {
	main_term(EXIT_FAILURE, "Error on closing file %s.\n", file);
    }

    return src;
}

void util_unload(char* data) {
    free(data);
}
