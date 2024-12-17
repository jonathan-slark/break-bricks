/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#include <ctype.h>
#include <glad.h>
#include <stdio.h>
#include <stdlib.h>

#include "sprite.h"
#include "level.h"
#include "main.h"
#include "util.h"

static Brick *bricks;
static unsigned int brickcount;
static Sprite brick = {
    .verts = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f
    },
    .texverts = {
        0,  0,
        31, 0,
        0,  9,
        31, 9
    },
    .width = 32,
    .height = 10,
    .xpos = 0,
    .ypos = 0
};

unsigned int
countbricks(const char *f)
{
    char c;
    unsigned count = 0;

    while ((c = *f++) != '\0')
	if (isdigit(c))
	    count++;

    return count;
}

void
readbricks(const char *f)
{
    char c;

    while ((c = *f++) != '\0') {
	if (c == '#') {
	    while ((c = *f++) != '\n') {
		/* Comment */
	    }
	} else if (c == '\n') {
	    /* New line */
	} else if (isdigit(c)) {
	    // TODO
	} else {
	    term(EXIT_FAILURE, "Syntax error in level file.\n");
	}
    }
}

void
level_load(const char *lvl, unsigned int width, unsigned int height)
{
    char *f;

    f = load(lvl);
    brickcount = countbricks(f);
    fprintf(stderr, "brickcount = %u\n", brickcount);
    bricks = (Brick *) malloc(brickcount * sizeof(Brick));
    readbricks(f);
    unload(f);

    sprite_init(&brick);
}

void
level_unload(void)
{
    sprite_term(&brick);
    free(bricks);
}

void
level_draw(GLuint shader)
{
    sprite_draw(shader, &brick);
}

int
level_iscompleted(void)
{
    return 0;
}
