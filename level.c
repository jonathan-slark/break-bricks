/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#define GLFW_INCLUDE_NONE
#include <ctype.h>
#include <cglm/struct.h>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "sprite.h"
#include "level.h"
#include "main.h"
#include "util.h"

#include "config.h"

/* Function prototypes */
void initbrick(Brick *brick, char id, unsigned int row, unsigned int col);
unsigned int readbricks(const char *lvl, Brick *bricks);

/* Variables */
static Brick *bricks;
static unsigned int brickcount;

/* Function implementations */

void
initbrick(Brick *brick, char id, unsigned int row, unsigned int col)
{
    Sprite *s = &brick->sprite;
    int solid;
    unsigned int i;

    solid = (!isdigit(id));
    if (solid)
	i = id - 'a' + bricktypes;
    else
	i = id - '0';

    brick->issolid = solid;
    brick->isdestroyed = 0;
    memcpy(s->texverts, brickverts[i], sizeof(brickverts[i]));

    s->size.x = brickwidth;
    s->size.y = brickheight;
    s->pos.x  = row * brickwidth;
    s->pos.y  = col * brickheight;

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
