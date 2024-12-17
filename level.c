/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#include <cglm/struct.h>
#include <ctype.h>
#include <glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "sprite.h"
#include "level.h"
#include "main.h"
#include "util.h"

/* Function prototypes */
void initbrick(Brick *brick, unsigned int id, unsigned int row,
	unsigned int col);
unsigned int readbricks(const char *l, Brick *bricks);

/* Variables */
static Brick *bricks;
static unsigned int brickcount;
Sprite sprite = {
    .texverts = {
	0,              0,
	brickwidth - 1, 0,
	0,              brickheight - 1,
	brickwidth - 1, brickheight - 1
    },
    .size = {{ (float) brickwidth, (float) brickheight }}
};

/* Function implementations */

void
initbrick(Brick *brick, unsigned int id, unsigned int row, unsigned int col)
{
    brick->row = row;
    brick->col = col;
    brick->colour = brickcolours[id];
    brick->isdestroyed = 0;
}

unsigned int
readbricks(const char *l, Brick *bricks)
{
    char c;
    unsigned count = 0, row = 0, col = 0;

    while ((c = *l++) != '\0') {
	if (c == '#') {
	    while ((c = *l++) != '\n') {
		/* Comment */
	    }
	} else if (c == 'x') {
	    /* No brick */
	    row++;
	} else if (isdigit(c)) {
	    if (bricks)
		initbrick(&bricks[count], (unsigned int) (c - '0'), row, col);
	    count++;
	    row++;
	} else if (c == '\n') {
	    col++;
	    row = 0;
	} else if (c != ' ' && c != '\t') {
	    term(EXIT_FAILURE, "Syntax error in level file.\n");
	}
    }

    return count;
}

void
level_load(const char *name)
{
    char *l;

    l = load(name);
    brickcount = readbricks(l, NULL);
    fprintf(stderr, "brickcount = %u\n", brickcount);
    bricks = (Brick *) malloc(brickcount * sizeof(Brick));
    readbricks(l, bricks);
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
    unsigned int i;
    Brick *b;

    for (i = 0; i < brickcount; i++) {
	b = &bricks[i];
	if (!b->isdestroyed) {
	    sprite.pos.x = b->row * brickwidth;
	    sprite.pos.y = b->col * brickheight;
	    sprite.colour = b->colour;
	    sprite_draw(shader, &sprite);
	}
    }
}

int
level_iscompleted(void)
{
    unsigned int i;

    for (i = 0; i < brickcount; i++)
	if (!bricks[i].isdestroyed)
	    return 0;

    return 1;
}
