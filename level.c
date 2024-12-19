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
static void initbrick(Brick *brick, char id, unsigned int row, unsigned int col);
static unsigned int readbricks(const char *lvl, Brick *bricks);
static inline void createaabb(vec2 aabb[2], Sprite *s);
/* static int checkaabb(Sprite *a, Sprite *b); */
static int checkball(Sprite *b, Sprite *s);

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

inline void
createaabb(vec2 aabb[2], Sprite *s)
{
    aabb[0][0] = s->pos.x;
    aabb[0][1] = s->pos.y;
    aabb[1][0] = s->pos.x + s->size.x;
    aabb[1][1] = s->pos.y + s->size.y;
}

/* Check collision between two sprites using cglm's AABB functions */
#if 0
int
checkaabb(Sprite *a, Sprite *b)
{
    vec2 aabba[2], aabbb[2];

    createaabb(aabba, a);
    createaabb(aabbb, b);

    return glm_aabb2d_aabb(aabba, aabbb);
}
#endif

/* Check collision between the ball and a sprite using cglm */
int
checkball(Sprite *b, Sprite *s)
{
    vec2s centre = glms_vec2_adds(b->pos, ballradius);
    vec3 circle = { centre.x, centre.y, ballradius };
    vec2 aabb[2];

    createaabb(aabb, s);

    return glm_aabb2d_circle(aabb, circle);
}

void
level_breakbricks(Sprite *ball)
{
    unsigned int i;

    for (i = 0; i < brickcount; i++)
	if (!bricks[i].isdestroyed)
	    if (checkball(ball, &bricks[i].sprite))
		if (!bricks[i].issolid)
		    bricks[i].isdestroyed = 1;
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
