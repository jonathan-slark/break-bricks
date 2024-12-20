/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#define GLFW_INCLUDE_NONE
#include <cglm/struct.h>
#include <ctype.h>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "main.h"
#include "sprite.h"
#include "util.h"

/* Types */

enum { Up, Right, Down, Left };

typedef struct {
    int key;
    void (*func)(float dt);
} Key;

typedef struct {
    int issolid;
    int isdestroyed;
    Sprite sprite;
} Brick;

typedef struct {
    vec2s vel;
    int isstuck;
    Sprite sprite;
} Ball;

/* Function prototypes */
static void initbrick(Brick *brick, char id, unsigned int row, unsigned int col);
static unsigned int readbricks(const char *lvl, Brick *bricks);
static void levelload(const char *lvl);
static void initball(void);
static void initpaddle(void);
static void levelunload(void);
static void movepaddle(float vel);
static void movepaddleleft(float dt);
static void movepaddleright(float dt);
static void releaseball(float dt);
static void moveball(float dt);
static int getdirection(vec2s vec);
static inline void createaabb(vec2 aabb[2], Sprite *s);
static int  checkball(Sprite *b, Sprite *s);
static void levelbreakbricks(Sprite *ball);
static int leveliscompleted(void);
static void leveldraw(GLuint shader);

/* Variables */
static int keypressed[GLFW_KEY_LAST + 1];
static GLuint spritesheet;
static GLuint spriteshader;
static Sprite paddle;
static Ball ball;
static const vec2s compass[] = {
    {{  0.0f, 1.0f  }},
    {{  1.0f, 0.0f  }},
    {{  0.0f, -1.0f }},
    {{ -1.0f, 0.0f  }}
};
static Brick *bricks;
static unsigned int brickcount;

/* Config uses types from this file */
#include "main_config.h"
#include "game_config.h"

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
levelload(const char *name)
{
    char *lvl;

    lvl = load(name);
    brickcount = readbricks(lvl, NULL);
    bricks = (Brick *) malloc(brickcount * sizeof(Brick));
    readbricks(lvl, bricks);
    unload(lvl);
}

void
initball(void)
{
    Sprite *s = &ball.sprite;
    vec2s offset = {{ paddlewidth / 2.0f - ballradius, -ballradius * 2.0f }};;

    ball.vel = ballvelocity;
    ball.isstuck = 1;

    memcpy(s->texverts, ballverts, sizeof(ballverts));
    s->size.x = ballwidth;
    s->size.y = ballheight;
    s->pos = glms_vec2_add(paddle.pos, offset);

    sprite_init(s);
}

void
initpaddle(void)
{
    Sprite *s = &paddle;

    memcpy(s->texverts, paddleverts, sizeof(paddleverts));
    s->size.x = paddlewidth;
    s->size.y = paddleheight;
    s->pos.x = scrwidth / 2.0f - s->size.x / 2.0f;
    s->pos.y = scrheight - s->size.y;

    sprite_init(s);
}

void
game_load(void)
{
    mat4s proj;
    char lvl[] = LVLFOLDER "/00.txt";
    char fmt[] = LVLFOLDER "/%02i.txt";

    proj = glms_ortho(0.0f, scrwidth, scrheight, 0.0f, -1.0f, 1.0f);
    spriteshader = sprite_shaderload(vertshader, fragshader);
    sprite_shaderuse(spriteshader);
    sprite_shadersetmat4s(spriteshader, projuniform, proj);

    spritesheet = sprite_sheetload(spritefile, 1);
    glActiveTexture(GL_TEXTURE0);
    sprite_sheetuse(spritesheet);
    sprite_shadersetint(spriteshader, texuniform, 0);

    sprintf(lvl, fmt, 1);
    levelload(lvl);
    initpaddle();
    initball();
}

void
levelunload(void)
{
    unsigned int i;

    for (i = 0; i < brickcount; i++)
	sprite_term(&bricks[i].sprite);

    free(bricks);
    brickcount = 0;
}

void
game_unload(void)
{
    sprite_term(&ball.sprite);
    sprite_term(&paddle);
    levelunload();
    sprite_sheetunload(spritesheet);
    sprite_shaderunload(spriteshader);
}

void
game_keydown(int key)
{
    keypressed[key] = 1;
}

void
game_keyup(int key)
{
    keypressed[key] = 0;
}

void
movepaddle(float vel)
{
    paddle.pos.x += vel;
    paddle.pos.x = CLAMP(paddle.pos.x, 0.0f, scrwidth - paddlewidth);

    if (ball.isstuck)
	ball.sprite.pos.x = paddle.pos.x + paddlewidth / 2.0f - ballradius;
}

void
movepaddleleft(float dt)
{
    movepaddle(-paddlevelocity * dt);
}

void
movepaddleright(float dt)
{
    movepaddle(paddlevelocity * dt);
}

void
releaseball(float dt)
{
    UNUSED(dt);

    ball.isstuck = 0;
}

void
game_input(float dt)
{
    size_t i;

    for (i = 0; i < COUNT(keys); i++)
	if (keypressed[keys[i].key])
	    (*keys[i].func)(dt);
}

void
moveball(float dt)
{
    Sprite *s = &ball.sprite;

    if (ball.isstuck)
	return;

    s->pos = glms_vec2_add(s->pos, glms_vec2_scale(ball.vel, dt));

    if (s->pos.x <= 0.0f) {
	ball.vel.x = -ball.vel.x;
	s->pos.x = 0.0f;
    } else if (s->pos.x + ballwidth >= scrwidth) {
	ball.vel.x = -ball.vel.x;
	s->pos.x = scrwidth - ballwidth;
    }

    if (s->pos.y <= 0.0f) {
	ball.vel.y = -ball.vel.y;
	s->pos.y = 0.0f;
    }
}

int
getdirection(vec2s vec)
{
    float max = 0.0f, dot;
    int bestmatch = -1;
    size_t i;

    for (i = 0; i < COUNT(compass); i++) {
	dot = glms_vec2_dot(glms_vec2_normalize(vec), compass[i]);
	if (dot > max) {
	    max = dot;
	    bestmatch = i;
	}
    }

    return bestmatch;
}

inline void
createaabb(vec2 aabb[2], Sprite *s)
{
    aabb[0][0] = s->pos.x;
    aabb[0][1] = s->pos.y;
    aabb[1][0] = s->pos.x + s->size.x;
    aabb[1][1] = s->pos.y + s->size.y;
}

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
levelbreakbricks(Sprite *ball)
{
    unsigned int i;

    for (i = 0; i < brickcount; i++)
	if (!bricks[i].isdestroyed)
	    if (checkball(ball, &bricks[i].sprite))
		if (!bricks[i].issolid)
		    bricks[i].isdestroyed = 1;
}

int
leveliscompleted(void)
{
    unsigned int i;

    for (i = 0; i < brickcount; i++)
	if (!bricks[i].issolid && !bricks[i].isdestroyed)
	    return 0;

    return 1;
}

void
game_update(float dt)
{
    moveball(dt);
    levelbreakbricks(&ball.sprite); 
}

void
leveldraw(GLuint shader)
{
    unsigned int i;

    for (i = 0; i < brickcount; i++)
	if (!bricks[i].isdestroyed)
	    sprite_draw(shader, &bricks[i].sprite);
}

void
game_render(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    leveldraw(spriteshader);
    sprite_draw(spriteshader, &paddle);
    sprite_draw(spriteshader, &ball.sprite);
}
