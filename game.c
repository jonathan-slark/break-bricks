/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 *
 * TODO:
 * Different resolutions.
 * Use circle for ball and paddle collision detection.
 */

#define GLFW_INCLUDE_NONE
#include <cglm/struct.h>
#include <ctype.h>
#include <float.h>
#include <glad.h>
#include <math.h>
#include <miniaudio.h>
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "aud.h"
#include "game.h"
#include "gfx.h"
#include "main.h"
#include "util.h"

// Types

enum { StateMenu, StateRun, StatePause, StateWon } state = StateRun;

typedef struct {
    int key;
    void (*func)(void);
} Key;

typedef struct {
    int button;
    void (*func)(void);
} Button;

typedef struct {
    // First vetex in the quad is also the position of the sprite
    union {
        vec2s pos;
        Quad quad;
    };
    vec2s size;
} Sprite;

typedef struct {
    Quad     quad;
    Renderer render;
} Screen;

typedef struct {
    bool   is_stuck;
    vec2s  vel;
    vec3s  circle;
    Sprite sprite;
} Ball;

typedef struct {
    // Circle either end, box in middle
    vec3s  circle1, circle2;
    vec2s  aabb[IND_COUNT];
    Sprite sprite;
} Paddle;

typedef struct {
    bool   is_active;
    bool   is_solid;
    bool   is_destroyed;
    Sprite sprite;
} Brick;

typedef struct {
    Ball     ball;
    Paddle   paddle;
    Brick*   bricks;
    Renderer render;
} Sprites;

// Function prototypes

static void paddle_init(Paddle* p, Sprite* ps);
static void ball_init(Ball* b, Sprite* bs, Sprite* p);
static void brick_init(Brick* brick, char id, unsigned col, unsigned row);
static void level_read(const char* data);
static void level_load(unsigned num);
static void level_unload(void);
static void level_reset(void);
static void quit(void);
static void pause(void);
static unsigned random(unsigned min, unsigned max);
static void ball_release(void);
static vec2s get_wall_dist(Sprite* s);
static void ball_update_circle(Ball* b, Sprite* bs);
static void bounce(Ball* b, Sprite* bs, vec2s vel, vec2s dist,
	bool is_paddle);
static bool is_wall_hit(Sprite* s, vec2s newpos);
static bool is_oob(Sprite* s, vec2s newpos);
static bool is_paddle_hit(Ball* s, Paddle* p, vec2s newpos);
static void ball_move(Ball* b, Sprite* s, double frame_time);

// Requires above types and functions
#include "config.h"

// Constants
static const unsigned sprite_count = 200;

// Variables
static Screen bg;
static Sprites sprites;
static unsigned level = 1;

// Function implementations

void paddle_init(Paddle* p, Sprite* ps) {
    vec2s mouse_pos = main_get_mouse_pos();
    vec2s pos = {{ mouse_pos.x, SCR_HEIGHT - PADDLE_SIZE.t }};
    ps->quad = gfx_quad_create(&sprites.render, pos, PADDLE_SIZE,
	    PADDLE_OFFSET);
    ps->size = PADDLE_SIZE;

    p->circle1 = PADDLE_CIRCLE1;
    p->circle2 = PADDLE_CIRCLE2;
    memcpy(p->aabb, PADDLE_AABB, sizeof PADDLE_AABB);
}

void ball_init(Ball* b, Sprite* bs, Sprite* ps) {
    b->is_stuck = true;
    vec2s pos = {{
        ps->pos.x + ps->size.s / 2.0f - BALL_SIZE.s / 2.0f,
        ps->pos.y - BALL_SIZE.t
    }};
    bs->quad = gfx_quad_create(&sprites.render, pos, BALL_SIZE, BALL_OFFSET);
    bs->size = BALL_SIZE;

    b->circle = (vec3s) {{ pos.x, pos.y, BALL_SIZE.s / 2.0f }};
}

void brick_init(Brick* b, char id, unsigned col, unsigned row) {
    bool solid = (!isdigit(id));
    unsigned i;
    i = solid ? (unsigned) id - 'a' + BRICK_TYPES : (unsigned) id - '0';

    b->is_active = true;
    b->is_solid = solid;
    b->is_destroyed = false;

    vec2s pos = {{
        BG_WALL + col * BRICK_SIZE.s,
        BG_WALL + row * BRICK_SIZE.t
    }};
    b->sprite.quad = gfx_quad_create(&sprites.render, pos, BRICK_SIZE,
	    BRICK_OFFSETS[i]);
}

void level_read(const char* data) {
    unsigned count = 0, col = 0, row = 0;

    char c = '\0';
    while ((c = *data++) != '\0') {
        if (c == '#') {
            while ((c = *data++) != '\n') {
                // Comment
            }
        } else if (c == 'x') {
            // No brick
            sprites.bricks[count++].is_active = false;
            col++;
        } else if (isdigit(c) || (c >= 'a' && c <= 'f')) {
            brick_init(&sprites.bricks[count++], c, col, row);
            col++;
        } else if (c == '\n') {
            // Ignore blank line
            if (col > 0)
                row++;
            col = 0;
        } else if (c != ' ' && c != '\t') {
            main_term(EXIT_FAILURE, "Syntax error in level file.\n");
        }
    }

    if (count != BRICK_COLS * BRICK_ROWS) {
        main_term(EXIT_FAILURE, "Incorrect number of bricks in level file.\n");
    }
}

void level_load(unsigned num) {
    char fmt[] = "%s/%02i.txt";
    int sz = snprintf(NULL, 0, fmt, LEVEL_FOLDER, num);
    char file[sz + 1];
    snprintf(file, sizeof file, fmt, LEVEL_FOLDER, num);
    char* data = util_load(file);

    sprites.bricks = (Brick*) malloc(BRICK_COLS * BRICK_ROWS * sizeof(Brick));
    level_read(data);

    util_unload(data);
}

void game_load(void) {
    gfx_init();

    // Decent random seed: https://stackoverflow.com/q/58150771
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    srand(ts.tv_nsec);

    bg.render = gfx_render_create(1, BG_FILE);
    bg.quad   = gfx_quad_create(&bg.render, BG_OFFSET, BG_SIZE, BG_OFFSET);

    sprites.render = gfx_render_create(sprite_count, SPRITE_SHEET);
    paddle_init(&sprites.paddle, &sprites.paddle.sprite);
    ball_init(&sprites.ball, &sprites.ball.sprite, &sprites.paddle.sprite);
    level_load(level);

    state = StateRun;
}

void level_unload(void) {
    if (sprites.bricks) free(sprites.bricks);
}

void level_reset(void) {
    level_unload();

    ball_init(&sprites.ball, &sprites.ball.sprite, &sprites.paddle.sprite);
    level_load(level);
}

void game_unload(void) {
    level_unload();
    gfx_render_delete(&sprites.render);
    gfx_render_delete(&bg.render);
    gfx_term();
}

void quit(void) {
    main_quit();
}

void pause(void) {
    switch(state) {
    case StateRun:
	state = StatePause;
	break;
    case StatePause:
	state = StateRun;
	break;
    default:
	// VOID
    };
}

void game_key_down(int key) {
    for (unsigned i = 0; i < COUNT(KEYS); i++) {
        if (KEYS[i].key == key) {
            (*KEYS[i].func)();
        }
    }
}

void game_key_up([[maybe_unused]] int key) {
    // VOID
}

// Random number between min and max, closed interval
unsigned random(unsigned min, unsigned max) {
    return roundf(min + ((float) rand()) / RAND_MAX * (max - min));
}

void ball_release(void) {
    Ball* b = &sprites.ball;

    if (b->is_stuck) {
        b->vel = BALL_RELEASE[random(0, COUNT(BALL_RELEASE) - 1)];
        b->vel = glms_vec2_normalize(b->vel);

        b->is_stuck = false;
    }
}

void game_button_down(int button) {
    for (size_t i = 0; i < COUNT(BUTTONS); i++) {
        if (BUTTONS[i].button == button) {
            (*BUTTONS[i].func)();
        }
    }
}

void game_button_up([[maybe_unused]] int button) {
    // VOID
}

void game_input([[maybe_unused]] double frame_time) {
    switch(state) {
    case StateRun:
	Sprite* ps = &sprites.paddle.sprite;
	Sprite* bs = &sprites.ball.sprite;

	vec2s mouse_pos = main_get_mouse_pos();
	mouse_pos.x = CLAMP(
		mouse_pos.x,
		BG_WALL,
		SCR_WIDTH - ps->size.s - BG_WALL
	);
	vec2s newpos = (vec2s) {{ mouse_pos.x, ps->pos.y }};
	gfx_quad_set_pos(&ps->quad, newpos, ps->size);

        if (sprites.ball.is_stuck) {
            vec2s newpos = (vec2s) {{
		ps->pos.x + ps->size.s / 2.0f - bs->size.s / 2.0f,
		bs->pos.y
            }};
            gfx_quad_set_pos(&bs->quad, newpos, bs->size);
        }

	// Don't allow cursor to move away from paddle
	main_set_mouse_pos(mouse_pos);
	break;
    default:
	// VOID
    }
}

vec2s get_wall_dist(Sprite* s) {
    float left  = s->pos.x - BG_WALL;
    float right = SCR_WIDTH - BG_WALL - s->size.s - s->pos.x;
    float top   = s->pos.y - BG_WALL;
    return (vec2s) {{ left < right ? left : right, top }};
}

void ball_update_circle(Ball* b, Sprite* bs) {
    b->circle.x = bs->pos.x;
    b->circle.y = bs->pos.y;
}

void bounce(Ball* b, Sprite* bs, vec2s vel, vec2s dist, bool is_paddle) {
    // Calculate which axis will hit first
    float time_x   = vel.x != 0.0f ? fabs(dist.s / vel.x) : 0.0f;
    float time_y   = vel.y != 0.0f ? fabs(dist.t / vel.y) : 0.0f;
    float time_min = fmin(time_x, time_y);

    // Move ball to the point of the collision
    vel = glms_vec2_scale(vel, time_min);
    gfx_quad_add_vec(&bs->quad, vel);
    ball_update_circle(b, bs);
    glm_vec2_print(bs->pos.raw, stderr);

    // Finally, bounce the ball
    if (time_x == time_y) {
        b->vel.x = -b->vel.x;
        b->vel.y = -b->vel.y;
    } else if (time_x < time_y) {
        b->vel.x = -b->vel.x;
    } else {
        // Adjust horizontal velocity based on distance from the paddles centre
        if (is_paddle) {
	    Sprite* ps = &sprites.paddle.sprite;

            float centre = ps->pos.x + ps->size.s / 2.0f;
            float dist = bs->pos.x + bs->size.s / 2.0f - centre;
            float percent = dist / (ps->size.s / 2.0f);
            b->vel.x = percent * BALL_BOUNCE_STR;
            b->vel.y = -b->vel.y;
            b->vel = glms_vec2_normalize(b->vel);
        } else {
            b->vel.y = -b->vel.y;
        }
    }
}

bool is_wall_hit(Sprite* s, vec2s newpos) {
    return newpos.x < BG_WALL ||
           newpos.x > SCR_WIDTH - BG_WALL - s->size.s ||
           newpos.y < BG_WALL;
}

bool is_oob(Sprite* s, vec2s newpos) {
    return newpos.y + s->size.x > SCR_HEIGHT;
}

bool is_paddle_hit([[maybe_unused]] Ball* b, [[maybe_unused]] Paddle* p, [[maybe_unused]] vec2s newpos) {
    return true;
}

vec2s get_paddle_dist([[maybe_unused]] Sprite* bs) {
    return (vec2s) {{ 0, 0 }};
}

void ball_move(Ball* b, Sprite* bs, double frame_time) {
    vec2s vel = glms_vec2_scale(b->vel, BALL_SPEED * frame_time);
    vec2s newpos = glms_vec2_add(bs->pos, vel);

    if (is_oob(bs, newpos)) {
	level_reset();
    } else if (is_wall_hit(bs, newpos)) {
        vec2s dist = get_wall_dist(bs);
        bounce(b, bs, vel, dist, false);
    } else if (is_paddle_hit(b, &sprites.paddle, newpos)) {
        vec2s dist = get_paddle_dist(bs);
        bounce(b, bs, vel, dist, true);
    } else {
	gfx_quad_set_pos(&bs->quad, newpos, bs->size);
	ball_update_circle(b, bs);
    }
}

void game_update(double frame_time) {
    switch(state) {
    case StateRun:
	if (!sprites.ball.is_stuck) {
	    Ball* b = &sprites.ball;

	    double cr_time = frame_time / CR_COUNT;
	    for (unsigned i = 0; i < CR_COUNT - 1; i++) {
		ball_move(b, &b->sprite, cr_time);
	    }
	    // Account for rounding errors
	    ball_move(b, &b->sprite, frame_time - cr_time * (CR_COUNT - 1));
	}
    default:
	// VOID
    }
}

void level_render(void) {
    for (unsigned i = 0; i < BRICK_COLS * BRICK_ROWS; i++) {
        if (sprites.bricks[i].is_active && !sprites.bricks[i].is_destroyed) {
            gfx_render_quad(&sprites.render, &sprites.bricks[i].sprite.quad);
        }
    }
}

void game_render(void) {
    gfx_render_begin(&bg.render);
    gfx_render_quad(&bg.render, &bg.quad);
    gfx_render_end(&bg.render);

    gfx_render_begin(&sprites.render);
    level_render();
    gfx_render_quad(&sprites.render, &sprites.paddle.sprite.quad);
    gfx_render_quad(&sprites.render, &sprites.ball.sprite.quad);
    gfx_render_end(&sprites.render);
}
