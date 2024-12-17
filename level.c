/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#include <ctype.h>
#include <glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sprite.h"
#include "level.h"
#include "main.h"
#include "util.h"

/* Function prototypes */
void initbrick(Brick *brick, unsigned int width, unsigned int height,
	unsigned int id);
unsigned int readbricks(const char *l, Brick *bricks, unsigned int width,
	unsigned int height);

/* Variables */
static Brick *bricks;
static unsigned int brickcount;
static float verts[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
};
#if 0
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
#endif

/* Function implementations */

void
initbrick(Brick *brick, unsigned int width, unsigned int height,
	unsigned int id)
{
    UNUSED(id);

    memcpy(brick->sprite.verts, verts, ARRAYCOUNT * sizeof(float));
    brick->sprite.width = width;
    brick->sprite.height = height;
    brick->sprite.xpos = 0;
    brick->sprite.ypos = 0;
}

unsigned int
readbricks(const char *l, Brick *bricks, unsigned int width,
	unsigned int height)
{
    char c;
    unsigned count = 0;

    while ((c = *l++) != '\0') {
	if (c == '#') {
	    while ((c = *l++) != '\n') {
		/* Comment */
	    }
	} else if (isdigit(c)) {
	    if (bricks)
		initbrick(&bricks[count], width, height,
			(unsigned int) (c - '0'));
	    count++;
	} else if (c != '\n' && c != ' ' && c != '\t') {
	    term(EXIT_FAILURE, "Syntax error in level file.\n");
	}
    }

    return count;
}

void
level_load(const char *name, unsigned int width, unsigned int height)
{
    char *l;

    l = load(name);
    brickcount = readbricks(l, NULL, 0, 0);
    fprintf(stderr, "brickcount = %u\n", brickcount);
    bricks = (Brick *) malloc(brickcount * sizeof(Brick));
    readbricks(l, bricks, width, height);
    unload(l);
}

void
level_unload(void)
{
    free(bricks);
    brickcount = 0;
}

void
level_draw(GLuint shader)
{
    UNUSED(shader);
    //sprite_draw(shader, &brick);
}

int
level_iscompleted(void)
{
    unsigned int i;

    for (i = 0; i < brickcount; i++)
	if (!bricks[i].issolid && !bricks[i].isdestroyed)
	    return 0;

    return 1;
}
