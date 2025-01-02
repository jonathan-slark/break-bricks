/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

#define GLFW_INCLUDE_NONE
#include <cglm/struct.h>
#include <cglm/struct/aabb2d.h>
#include <ctype.h>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "game.h"
#include "main.h"
#include "gfx.h"
#include "util.h"

/* Macros */
#define EXT(x) ((x) / 2.0f)

/* Types */

typedef struct {
    int key;
    void (*func)(double frametime);
} Key;

typedef struct {
    bool isstuck;
    vec2s v;
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
static void moveball(double frametime);
static void collisiondetect(void);
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
	ball.v = (vec2s) {{ random(-0.5f, 0.5f), -0.5f }};
	ball.v = glms_vec2_normalize(ball.v);

	ball.isstuck = 0;
    }
}

void game_input(double frametime) {
    for (size_t i = 0; i < COUNT(KEYS); i++)
	if (keypressed[KEYS[i].key])
	    (*KEYS[i].func)(frametime);
}

void moveball(double frametime) {
    Sprite* s=&ball.sprite;

    vec2s v = glms_vec2_scale(ball.v, BALL_MOVE * frametime);
    s->pos = glms_vec2_add(s->pos, v);

    unsigned left = WALL_WIDTH;
    unsigned right = SCR_WIDTH - WALL_WIDTH - BALL_WIDTH;
    unsigned top = WALL_WIDTH;

    if (s->pos.x <= left) {
	s->pos.x = left;
	ball.v.x = -ball.v.x;
    } else if (s->pos.x >= right) {
	s->pos.x = right;
	ball.v.x = -ball.v.x;
    }
    if (s->pos.y <= top) {
	s->pos.y = top;
	ball.v.y = -ball.v.y;
    }
}

void collisiondetect(void) {
    Sprite* b = &ball.sprite;

    /* Circle = x, y, radius */
    vec3s c = {{ b->pos.x + EXT(b->size.x), b->pos.y + EXT(b->size.y),
	EXT(b->size.x) }};
    vec2s aabb[2] = {
	{{ paddle.pos.x, paddle.pos.y }},
	{{ paddle.pos.x + paddle.size.x, paddle.pos.y + paddle.size.y }}
    };

    if (glms_aabb2d_circle(aabb, c)) {
	b->pos.y = paddle.pos.y - paddle.size.y;
	ball.v.y = -ball.v.y;
    }

    for (unsigned i = 0; i < brickcount; i++) {
	if (!bricks[i].isdestroyed) {
	    if (glms_aabb2d_circle(bricks[i].aabb, c)) {
		bricks[i].isdestroyed = true;
	    }
	}
    }
}

void game_update(double frametime) {
    if (!ball.isstuck) {
	moveball(frametime);
	collisiondetect();
    }
}

void leveldraw(void) {
    for (unsigned i = 0; i < brickcount; i++)
	if (!bricks[i].isdestroyed)
	    gfx_sprite_draw(&bricks[i].sprite);
}

void game_render(void) {
    gfx_ss_use(bg);
    gfx_sprite_draw(&bgsprite);

    gfx_ss_use(spritesheet);
    leveldraw();
    gfx_sprite_draw(&ball.sprite);
    gfx_sprite_draw(&paddle);
}
