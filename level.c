/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#include <cglm/struct.h>
#include <ctype.h>
#include <glad.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "sprite.h"
#include "level.h"
#include "main.h"
#include "util.h"

/* Function prototypes */
void initbrick(Brick *brick, char c, unsigned int row, unsigned int col);
unsigned int readbricks(const char *lvl, Brick *bricks);

/* Variables */
static Brick *bricks;
static unsigned int brickcount;

/* Function implementations */

void
initbrick(Brick *brick, char c, unsigned int row, unsigned int col)
{
    Sprite *s = &brick->sprite;
    int solid;
    unsigned int id, yoff;

    /* Solid bricks are on row below breakable version of same colour */
    solid = (!isdigit(c));
    if (solid) {
	id = (unsigned int) (c - 'a');
	yoff = brickheight;
    } else {
	id = (unsigned int) (c - '0');
	yoff = 0;
    }

    brick->issolid = solid;
    brick->isdestroyed = 0;

    /* Top left */
    s->texverts[0] = id * brickwidth;
    s->texverts[1] = yoff;
    /* Top right */
    s->texverts[2] = ((id + 1) * brickwidth) - 1;
    s->texverts[3] = yoff;
    /* Bottom left */
    s->texverts[4] = id * brickwidth;
    s->texverts[5] = yoff + brickheight - 1;
    /* Bottom right */
    s->texverts[6] = ((id + 1) * brickwidth) - 1;
    s->texverts[7] = yoff + brickheight - 1;

    s->size.x = (float) brickwidth;
    s->size.y = (float) brickheight;
    s->pos.x  = (float) (row * brickwidth);
    s->pos.y  = (float) (col * brickheight);

    sprite_init(s);
}

/* Run once with bricks = NULL to get the brick count, a second time with
 * bricks pointing to an array of bricks to be initialised. */
unsigned int
readbricks(const char *lvl, Brick *bricks)
{
    char c;
    unsigned count = 0, row = 0, col = 0;

    while ((c = *lvl++) != '\0') {
	if (c == '#') {
	    while ((c = *lvl++) != '\n') {
		/* Comment */
	    }
	} else if (c == 'x') {
	    /* No brick */
	    row++;
	} else if (isdigit(c) || (c >= 'a' && c <= 'f')) {
	    if (bricks)
		initbrick(&bricks[count], c, row, col);
	    count++;
	    row++;
	} else if (c == '\n') {
	    /* Ignore blank line */
	    if (row > 0)
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
    char *lvl;

    lvl = load(name);
    brickcount = readbricks(lvl, NULL);
    bricks = (Brick *) malloc(brickcount * sizeof(Brick));
    readbricks(lvl, bricks);
    unload(lvl);
}

void
level_unload(void)
{
    unsigned int i;

    for (i = 0; i < brickcount; i++)
	sprite_term(&bricks[i].sprite);

    free(bricks);
    brickcount = 0;
}

void
level_draw(GLuint shader)
{
    unsigned int i;

    for (i = 0; i < brickcount; i++)
	if (!bricks[i].isdestroyed)
	    sprite_draw(shader, &bricks[i].sprite);
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
