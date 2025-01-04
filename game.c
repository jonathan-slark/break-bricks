/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 *
 * A typical AAAB collision detection function, for reference:
bool isaabbcollision(Sprite* s1, Sprite* s2) {
    return s1->pos.x < s2->pos.x + s2->size.x &&
	   s1->pos.x + s1->size.x > s2->pos.x &&
	   s1->pos.y < s2->pos.y + s2->size.y &&
	   s1->pos.y + s1->size.y > s2->pos.y;
}
 */

#define GLFW_INCLUDE_NONE
#include <cglm/struct.h>
#include <cglm/struct/aabb2d.h>
#include <ctype.h>
#include <glad.h>
#include <math.h>
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "main.h"
#include "gfx.h"
#include "util.h"

/* Types */

typedef struct {
    int key;
    void (*func)(double frametime);
} Key;

typedef struct {
    bool isstuck;
    vec2s vel;
    Sprite sprite;
} Ball;

typedef struct {
    bool issolid;
    bool isdestroyed;
    Sprite sprite;
    vec2s aabb[2];
} Brick;

/* Function prototypes */
static void initsprite(Sprite* s, float width, float height, float x,
		       float y, float rot, const unsigned* verts, size_t size);
static void initbrick(Brick* brick, char id, unsigned row, unsigned col);
static unsigned readbricks(const char* lvl, Brick* bricks);
static void levelload(const char* lvl);
static void initball(void);
static void initpaddle(void);
static void levelunload(void);
static void movepaddle(float move);
static void movepaddleleft(double frametime);
static void movepaddleright(double frametime);
static float random(float min, float max);
static void releaseball(double frametime);
static bool iswallcollision(Sprite* s, vec2s newpos);
static vec2s getwalldistance(Sprite* s);
static void bounce(vec2s vel, vec2s dist);
static bool ispaddlecollision(Sprite* s, vec2s newpos);
static vec2s getaabbdistance(Sprite* s1, Sprite* s2);
static void moveball(double frametime);
static void leveldraw(void);

/* Variables */
static Brick* bricks = NULL;
static unsigned brickcount = 0;
static Ball ball = {};
static Sprite paddle = {};
static GLuint spritesheet = 0, bg = 0;
static Sprite bgsprite = {};
static bool keypressed[GLFW_KEY_LAST + 1] = {};

/* Uses types defined above */
#include "config.h"

/* Function implementations */

void initsprite(Sprite* s, float width, float height, float x,
		float y, float rot, const unsigned* verts, size_t size) {
    memcpy(s->texverts, verts, size);
    s->size.x = width;
    s->size.y = height;
    s->pos.x = x;
    s->pos.y = y;
    s->rot = rot;
    gfx_sprite_init(s);
}

void initbrick(Brick* brick, char id, unsigned row, unsigned col) {
    bool solid = (!isdigit(id));
    unsigned i;
    if (solid)
	i = id - 'a' + BRICK_TYPES;
    else
	i = id - '0';

    brick->issolid = solid;
    brick->isdestroyed = 0;

    float x = row * BRICK_WIDTH  + WALL_WIDTH;
    float y = col * BRICK_HEIGHT + WALL_WIDTH;
    initsprite(&brick->sprite, BRICK_WIDTH, BRICK_HEIGHT, x, y, 0.0f, BRICK_VERTS[i],
	       sizeof(BRICK_VERTS[i]));

    brick->aabb[0].x = x;
    brick->aabb[0].y = y;
    brick->aabb[1].x = x + BRICK_WIDTH;
    brick->aabb[1].y = y + BRICK_HEIGHT;
}

/* Run once with bricks = NULL to get the brick count, a second time with
 * bricks pointing to an array of bricks to be initialised. */
unsigned readbricks(const char* lvl, Brick* bricks) {
    unsigned count = 0, row = 0, col = 0;

    char c = '\0';
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

void levelload(const char* name) {
    char* lvl = util_load(name);
    brickcount = readbricks(lvl, NULL);
    bricks = (Brick*) malloc(brickcount * sizeof(Brick));
    readbricks(lvl, bricks);
    util_unload(lvl);
}

void initball(void) {
    ball.isstuck = 1;
    float x = paddle.pos.x + PADDLE_WIDTH / 2.0f - BALL_WIDTH / 2.0f;
    float y = paddle.pos.y - BALL_HEIGHT;
    initsprite(&ball.sprite, BALL_WIDTH, BALL_HEIGHT, x, y, 0.0f, BALL_VERTS,
	       sizeof(BALL_VERTS));
}

void initpaddle(void) {
    float x = SCR_WIDTH / 2.0f - PADDLE_WIDTH / 2.0f;
    float y = SCR_HEIGHT - PADDLE_HEIGHT;
    initsprite(&paddle, PADDLE_WIDTH, PADDLE_HEIGHT, x, y, 0.0f, PADDLE_VERTS,
	       sizeof(PADDLE_VERTS));
}

void game_load(void) {
    gfx_init();

    /* Decent random seed: https://stackoverflow.com/q/58150771 */
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    srand(ts.tv_nsec);

    spritesheet = gfx_ss_load(SPRITE_SHEET, 1);
    bg = gfx_ss_load(BACKGROUND, 1);
    initsprite(&bgsprite, SCR_WIDTH, SCR_HEIGHT, 0.0f, 0.0f, 0.0f, BG_VERTS,
	       sizeof(BG_VERTS));

    const char fmt[] = "%s/%02i.txt";
    int sz = snprintf(NULL, 0, fmt, LVL_FOLDER, 1);
    char lvl[sz + 1];
    snprintf(lvl, sizeof lvl, fmt, LVL_FOLDER, 1);
    levelload(lvl);

    initpaddle();
    initball();
}

void levelunload(void) {
    for (unsigned i = 0; i < brickcount; i++)
	gfx_sprite_term(&bricks[i].sprite);

    free(bricks);
    brickcount = 0;
}

void game_unload(void) {
    gfx_sprite_term(&ball.sprite);
    gfx_sprite_term(&paddle);
    levelunload();
    gfx_ss_unload(bg);
    gfx_ss_unload(spritesheet);
    gfx_term();
}

void game_keydown(int key) {
    keypressed[key] = true;
}

void game_keyup(int key) {
    keypressed[key] = false;
}

void movepaddle(float move) {
    paddle.pos.x = glm_clamp(paddle.pos.x + move, WALL_WIDTH,
	    SCR_WIDTH - PADDLE_WIDTH - WALL_WIDTH);

    if (ball.isstuck)
	ball.sprite.pos.x = paddle.pos.x + PADDLE_WIDTH / 2.0f -
	    BALL_WIDTH / 2.0f;
}

void movepaddleleft(double frametime) {
    movepaddle(PADDLE_MOVE * -frametime);
}

void movepaddleright(double frametime) {
    movepaddle(PADDLE_MOVE * frametime);
}

/* Random number between min and max, closed interval */
float random(float min, float max) {
    return min + ((float) rand()) / RAND_MAX * (max - min);
}

void releaseball([[maybe_unused]] double frametime) {
    if (ball.isstuck) {
	ball.vel = (vec2s) {{ random(-0.5f, 0.5f), -0.5f }};
	ball.vel = glms_vec2_normalize(ball.vel);

	ball.isstuck = 0;
    }
}

void game_input(double frametime) {
    for (size_t i = 0; i < COUNT(KEYS); i++)
	if (keypressed[KEYS[i].key])
	    (*KEYS[i].func)(frametime);
}

bool iswallcollision(Sprite* s, vec2s newpos) {
    return newpos.x < WALL_WIDTH ||
	   newpos.x > SCR_WIDTH - WALL_WIDTH - s->size.x ||
	   newpos.y < WALL_WIDTH;
}

vec2s getwalldistance(Sprite* s) {
    float left = s->pos.x - WALL_WIDTH;
    float right = SCR_WIDTH - WALL_WIDTH - s->size.x - s->pos.x;
    float top = s->pos.y - WALL_WIDTH;
    return (vec2s) {{ left < right ? left : right, top }};
}

bool ispaddlecollision(Sprite* s, vec2s newpos) {
    return newpos.y > SCR_HEIGHT - paddle.size.y - s->size.y &&
	   newpos.y < SCR_HEIGHT &&
	   newpos.x < paddle.pos.x + paddle.size.x &&
	   newpos.x + s->size.x > paddle.pos.x;
}

vec2s getaabbdistance(Sprite* s1, Sprite* s2) {
    vec2s dist = {};
    float s1x = s1->pos.x, s1y = s1->pos.y, s2x = s2->pos.x, s2y = s2->pos.y;

    if (s1x < s2x) {
	dist.x = s2x - (s1x + s1->size.x);
    } else if (s1x > s2x) {
	dist.x = s1x - (s2x + s2->size.x);
    }
    if (s1y < s2y) {
	dist.y = s2y - (s1y + s1->size.y);
    } else if (s1y > s2y) {
	dist.y = s1y - (s2y + s2->size.y);
    }

    return dist;
}

void bounce(vec2s vel, vec2s dist) {
    Sprite* s = &ball.sprite;

    // Calculate which axis will hit first
    float timex = vel.x != 0.0f ? fabs(dist.x / vel.x) : 0.0f;
    float timey = vel.y != 0.0f ? fabs(dist.y / vel.y) : 0.0f;
    float mintime = fmin(timex, timey);

    // Move ball to the point of the collision
    vel = glms_vec2_scale(vel, mintime);
    s->pos = glms_vec2_add(s->pos, vel);
#ifndef NDEBUG
    fprintf(stderr, "%s\n", "new pos = ");
    glms_vec2_print(s->pos, stderr);
#endif

    // Finally, bounce the ball
    if (timex < timey) {
	ball.vel.x = -ball.vel.x;
    } else {
	ball.vel.y = -ball.vel.y;
    }
}

void moveball(double frametime) {
    Sprite* s = &ball.sprite;
    vec2s vel = glms_vec2_scale(ball.vel, BALL_MOVE * frametime);
    vec2s newpos = glms_vec2_add(s->pos, vel);

    if (iswallcollision(s, newpos)) {
	vec2s dist = getwalldistance(s);
	bounce(vel, dist);
    } else if (ispaddlecollision(s, newpos)) {
	vec2s dist = getaabbdistance(s, &paddle);
	bounce(vel, dist);
    } else {
	s->pos = newpos;
    }
}

void game_update(double frametime) {
    if (!ball.isstuck) {
	moveball(frametime);
    }
}

void leveldraw(void) {
    for (unsigned i = 0; i < brickcount; i++) {
	if (!bricks[i].isdestroyed) {
	    gfx_sprite_draw(&bricks[i].sprite);
	}
    }
}

void game_render(void) {
    gfx_ss_use(bg);
    gfx_sprite_draw(&bgsprite);

    gfx_ss_use(spritesheet);
    leveldraw();
    gfx_sprite_draw(&ball.sprite);
    gfx_sprite_draw(&paddle);
}
