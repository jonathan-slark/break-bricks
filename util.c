/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#include <cglm/struct.h>
#include <stdio.h>

#include "util.h"

#ifndef NDEBUG

void
printmat4(mat4s m)
{
    int i, rows = 4;

    for (i = 0; i < rows; i++)
	fprintf(stderr, "%f %f %f %f\n", m.raw[i][0], m.raw[i][1], m.raw[i][2],
		m.raw[i][3]);
}

#endif /* !NDEBUG */
