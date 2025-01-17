/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 *
 * TODO:
 * Different resolutions.
 * Use circle for ball and paddle collision detection.
 * Quadsheet struct.
 * Quad file?
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

#include "main.h"
#include "aud.h"
#include "game.h"
#include "gfx.h"
#include "util.h"

// Types

enum { SoundBrick, SoundDeath, SoundMusic, SoundWin };

typedef struct {
    int key;
    void (*func)(void);
} Key;

typedef struct {
    int button;
    void (*func)(void);
} Button;

typedef struct {
    bool isstuck;
    vec2s vel;
    Quad quad;
} Ball;

typedef struct {
    bool isactive;
    bool issolid;
    bool isdestroyed;
    Quad quad;
} Brick;

// Function prototypes
static void initbrick(Brick* brick, char id, unsigned col, unsigned row);
static unsigned readbricks(const char* lvl);
static void resetlevel(void);
static void levelload(unsigned num);
static void initball(void);
static void initpaddle(void);
static void levelunload(void);
static unsigned random(unsigned min, unsigned max);
static void releaseball(void);
static void quit(void);
static void pause(void);
static bool iswallcollision(Quad* q, vec2s newpos);
static vec2s getwalldistance(Quad* q);
static void bounce(Quad* q, vec2s vel, vec2s dist, bool ispaddle);
static bool ispaddlecollision(Quad* q, vec2s newpos);
static vec2s getaabbdistance(Quad* q1, Quad* q2);
static unsigned getbrickindex(float x, float y);
static unsigned brickcollisioncount(Quad* q, vec2s newpos,
	unsigned hitbricks[4]);
static vec2s getbrickdistance(Quad* q, unsigned hitcount,
	unsigned hitbricks[4]);
static bool isoob(Quad* q, vec2s newpos);
static void moveball(double frametime);
static bool iswincondition(void);

// Constants
static const size_t quad_count = 200;
static const size_t bg_count     = 1;

// Variables
static Brick* bricks;
static Ball ball;
static Quad paddle;
static Tex tex_quad, tex_bg;
static Renderer render_quad, render_bg;
static Quad bg;
static unsigned level = 1;
static ma_sound** sounds;
static bool ispaused = false;

// Uses types defined above
#include "config.h"

// Function implementations

void initbrick(Brick* brick, char id, unsigned col, unsigned row) {
    bool solid = (!isdigit(id));
    unsigned i;
    if (solid)
	i = id - 'a' + BRICK_TYPES;
    else
	i = id - '0';

    brick->isactive = true;
    brick->issolid = solid;
    brick->isdestroyed = 0;

    vec2s pos = {{
	WALL_WIDTH + col * BRICK_SIZE.s,
	WALL_WIDTH + row * BRICK_SIZE.t
    }};
    brick->quad = gfx_quad_create(pos, BRICK_SIZE, BRICK_OFFSETS[i], tex_quad);
}

unsigned readbricks(const char* lvl) {
    unsigned count = 0, col = 0, row = 0;

    char c = '\0';
    while ((c = *lvl++) != '\0') {
	if (c == '#') {
	    while ((c = *lvl++) != '\n') {
		// Comment
	    }
	} else if (c == 'x') {
	    // No brick
	    bricks[count++].isactive = false;
	    col++;
	} else if (isdigit(c) || (c >= 'a' && c <= 'f')) {
	    initbrick(&bricks[count++], c, col, row);
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

    return count;
}

void levelload(unsigned num) {
    const char fmt[] = "%s/%02i.txt";
    int sz = snprintf(NULL, 0, fmt, LVL_FOLDER, num);
    char file[sz + 1];
    snprintf(file, sizeof file, fmt, LVL_FOLDER, num);
    char* lvl = util_load(file);

    bricks = (Brick*) malloc(BRICK_COLS * BRICK_ROWS * sizeof(Brick));
    readbricks(lvl);

    util_unload(lvl);
}

void initball(void) {
    ball.isstuck = 1;
    vec2s pos = {{
	gfx_quad_pos(&paddle).x + gfx_quad_size(&paddle).x / 2.0f - BALL_SIZE.s / 2.0f,
	gfx_quad_pos(&paddle).y - BALL_SIZE.t
    }};
    ball.quad = gfx_quad_create(pos, BALL_SIZE, BALL_OFFSET, tex_quad);
}

void initpaddle(void) {
    vec2s mousepos = main_getmousepos();
    vec2s pos = {{ mousepos.x, SCR_HEIGHT - PADDLE_SIZE.t }};
    paddle = gfx_quad_create(pos, PADDLE_SIZE, PADDLE_OFFSET, tex_quad);
}

void game_load(void) {
    aud_init(AUD_VOL);
    gfx_init();

    // Decent random seed: https://stackoverflow.com/q/58150771
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    srand(ts.tv_nsec);

    tex_quad = gfx_tex_load(SPRITE_SHEET);
    tex_bg   = gfx_tex_load(BACKGROUND);

    vec2s pos = {{ 0, 0 }};
    bg = gfx_quad_create(pos, tex_bg.size, pos, tex_bg);

    sounds = (ma_sound**) malloc((SoundWin + 1) * sizeof(ma_sound*));
    sounds[SoundBrick] = aud_sound_load(AUD_BRICK, false);
    sounds[SoundDeath] = aud_sound_load(AUD_DEATH, false);
    sounds[SoundMusic] = aud_sound_load(AUD_MUSIC, true);  // Looping
    sounds[SoundWin]   = aud_sound_load(AUD_WIN, false);
    aud_sound_start(sounds[SoundMusic]);

    levelload(level);
    initpaddle();
    initball();

    render_quad = gfx_render_create(quad_count, tex_quad);
    render_bg   = gfx_render_create(bg_count,   tex_bg);
}

void resetlevel(void) {
    levelunload();

    levelload(level);
    initpaddle();
    initball();
}

void levelunload(void) {
    free(bricks);
}

void game_unload(void) {
    gfx_render_delete(&render_bg);
    gfx_render_delete(&render_quad);
    aud_sound_unload(sounds[SoundBrick]);
    aud_sound_unload(sounds[SoundDeath]);
    aud_sound_unload(sounds[SoundWin]);
    free(sounds);
    levelunload();
    gfx_tex_unload(tex_bg);
    gfx_tex_unload(tex_quad);
    gfx_term();
    aud_term();
}

void game_keydown(int key) {
    for (size_t i = 0; i < COUNT(KEYS); i++) {
	if (KEYS[i].key == key) {
	    (*KEYS[i].func)();
	}
    }
}

void game_keyup([[maybe_unused]] int key) {
    // VOID
}

void game_buttondown(int button) {
    for (size_t i = 0; i < COUNT(BUTTONS); i++) {
	if (BUTTONS[i].button == button) {
	    (*BUTTONS[i].func)();
	}
    }
}

void game_buttonup([[maybe_unused]] int button) {
    // VOID
}

// Random number between min and max, closed interval
unsigned random(unsigned min, unsigned max) {
    return roundf(min + ((float) rand()) / RAND_MAX * (max - min));
}

void releaseball(void) {
    if (ball.isstuck) {
	ball.vel = BALL_RELEASE[random(0, COUNT(BALL_RELEASE) - 1)];
	ball.vel = glms_vec2_normalize(ball.vel);

	ball.isstuck = 0;
    }
}

void quit(void) {
    main_quit();
}

void pause(void) {
    ispaused = !ispaused;
}

void game_input([[maybe_unused]] double frametime) {
    if (!ispaused) {
	vec2s mousepos = main_getmousepos();
	mousepos.x = CLAMP(mousepos.x, WALL_WIDTH,
		SCR_WIDTH - gfx_quad_size(&paddle).x - WALL_WIDTH);
	vec2s newpos = (vec2s) {{ mousepos.x, 0 }};
	gfx_quad_move(&paddle, newpos);

	if (ball.isstuck) {
	    vec2s newpos = (vec2s) {{ gfx_quad_pos(&paddle).x +
		gfx_quad_size(&paddle).x / 2.0f -
		    gfx_quad_size(&ball.quad).x / 2.0f, 0 }};
	    gfx_quad_move(&ball.quad, newpos);
	}

	// Don't allow cursor to move away from paddle
	main_setmousepos(mousepos);
    }
}

bool iswallcollision(Quad* q, vec2s newpos) {
    return newpos.x < WALL_WIDTH ||
	   newpos.x > SCR_WIDTH - WALL_WIDTH - gfx_quad_pos(q).x ||
	   newpos.y < WALL_WIDTH;
}

vec2s getwalldistance(Quad* q) {
    float left  = gfx_quad_pos(q).x - WALL_WIDTH;
    float right = SCR_WIDTH - WALL_WIDTH - gfx_quad_pos(q).x - gfx_quad_pos(q).x;
    float top   = gfx_quad_pos(q).y - WALL_WIDTH;
    return (vec2s) {{ left < right ? left : right, top }};
}

void bounce(Quad* q, vec2s vel, vec2s dist, bool ispaddle) {
    // Calculate which axis will hit first
    float timex = vel.x != 0.0f ? fabs(dist.x / vel.x) : 0.0f;
    float timey = vel.y != 0.0f ? fabs(dist.y / vel.y) : 0.0f;
    float mintime = fmin(timex, timey);

    // Move ball to the point of the collision
    vel = glms_vec2_scale(vel, mintime);
    gfx_quad_move(q, vel);

    // Finally, bounce the ball
    if (timex == timey) {
	ball.vel.x = -ball.vel.x;
	ball.vel.y = -ball.vel.y;
    } else if (timex < timey) {
	ball.vel.x = -ball.vel.x;
    } else {
	// Adjust horizontal velocity based on distance from the paddles centre
	if (ispaddle) {
	    float centre = gfx_quad_pos(&paddle).x + gfx_quad_size(&paddle).x / 2.0f;
	    float dist = (gfx_quad_pos(q).x + gfx_quad_pos(q).x / 2.0f) - centre;
	    float percent = dist / (gfx_quad_size(&paddle).x / 2.0f);
	    ball.vel.x = percent * BALL_BOUNCE_STR;
	    ball.vel.y = -ball.vel.y;
	    ball.vel = glms_vec2_normalize(ball.vel);
	} else {
	    ball.vel.y = -ball.vel.y;
	}
    }
}

bool ispaddlecollision(Quad* q, vec2s newpos) {
    return newpos.y > SCR_HEIGHT - gfx_quad_size(&paddle).y - gfx_quad_pos(q).y &&
	   newpos.y < SCR_HEIGHT &&
	   newpos.x < gfx_quad_pos(&paddle).x + gfx_quad_size(&paddle).x &&
	   newpos.x + gfx_quad_pos(q).x > gfx_quad_pos(&paddle).x;
}

vec2s getaabbdistance(Quad* q1, Quad* q2) {
    vec2s dist = {};
    float s1x = gfx_quad_pos(q1).x, s1y = gfx_quad_pos(q1).y,
	  s2x = gfx_quad_pos(q2).x, s2y = gfx_quad_pos(q2).y;

    if (s1x < s2x) {
	dist.x = s2x - (s1x + gfx_quad_size(q1).x);
    } else if (s1x > s2x) {
	dist.x = s1x - (s2x + gfx_quad_size(q2).x);
    }
    if (s1y < s2y) {
	dist.y = s2y - (s1y + gfx_quad_size(q1).y);
    } else if (s1y > s2y) {
	dist.y = s1y - (s2y + gfx_quad_size(q2).y);
    }

    return dist;
}

// Bricks are in a grid and don't move so we can get the index from a position
unsigned getbrickindex(float x, float y) {
    unsigned col = (x - WALL_WIDTH) / BRICK_SIZE.s;
    unsigned row = (y - WALL_WIDTH) / BRICK_SIZE.t;
    return col + row * BRICK_COLS;
}

// Check if any of the four corners of the ball hit a brick
unsigned brickcollisioncount(Quad* q, vec2s newpos, unsigned hitbricks[4]) {
    unsigned indices[4];
    unsigned hitbrickcount = 0;

    // Is the ball within the brick area?
    if (newpos.y < WALL_WIDTH + BRICK_ROWS * BRICK_SIZE.t) {
	indices[0] = getbrickindex(newpos.x, newpos.y);
	indices[1] = getbrickindex(newpos.x + gfx_quad_pos(q).x, newpos.y);
	indices[2] = getbrickindex(newpos.x, newpos.y + gfx_quad_pos(q).y);
	indices[3] = getbrickindex(newpos.x + gfx_quad_pos(q).x, newpos.y + gfx_quad_pos(q).y);
	for (unsigned i = 0; i < 4; i++) {
	    // Corners of the ball may evaluate to indices outside the boundary
	    unsigned j = indices[i];
	    if (j < BRICK_ROWS * BRICK_COLS && bricks[j].isactive &&
		    !bricks[j].isdestroyed) {
		hitbricks[hitbrickcount++] = j;
	    }
	}
    }

    return hitbrickcount;
}

// Find the distance to the closest brick and destroy it, if applicable
vec2s getbrickdistance(Quad* q, unsigned hitcount, unsigned hitbricks[4]) {
    float mindist = FLT_MAX;
    unsigned mini = 0;
    vec2s minv = {};

    for (unsigned i = 0; i < hitcount; i++) {
	vec2s v = getaabbdistance(q, &bricks[hitbricks[i]].quad);
	if (v.x < mindist && v.x < v.y) {
	    minv = v;
	    mindist = v.x;
	    mini = i;
	} else if (v.y < mindist) {
	    minv = v;
	    mindist = v.y;
	    mini = i;
	}
    }

    unsigned i = hitbricks[mini];
    if (!bricks[i].issolid) {
	bricks[i].isdestroyed = true;
	aud_sound_play(AUD_BRICK);
    }

    return minv;
}

bool isoob(Quad* q, vec2s newpos) {
    return newpos.y + gfx_quad_pos(q).x > SCR_HEIGHT;
}

void moveball(double frametime) {
    Quad* q      = &ball.quad;
    vec2s vel    = glms_vec2_scale(ball.vel, BALL_MOVE * frametime);
    vec2s newpos = glms_vec2_add(gfx_quad_pos(q), vel);

    if (isoob(q, newpos)) {
	aud_sound_play(AUD_DEATH);
	resetlevel();
    } else if (iswallcollision(q, newpos)) {
	vec2s dist = getwalldistance(q);
	bounce(q, vel, dist, false);
    } else if (ispaddlecollision(q, newpos)) {
	vec2s dist = getaabbdistance(q, &paddle);
	bounce(q, vel, dist, true);
    } else {
	unsigned hitbricks[4];
	unsigned hitcount = brickcollisioncount(q, newpos, hitbricks);
	if (hitcount > 0) {
	    vec2s dist = getbrickdistance(q, hitcount, hitbricks);
	    bounce(q, vel, dist, false);
	} else {
	    gfx_quad_move(q, vel);
	}
    }
}

bool iswincondition(void) {
    for (unsigned i = 0; i < BRICK_COLS * BRICK_ROWS; i++) {
	if (bricks[i].isactive && !bricks[i].issolid &&
		!bricks[i].isdestroyed) {
	    return false;
	}
    }

    return true;
}

void game_update(double frametime) {
    if (!ispaused && !ball.isstuck) {
	double restime = frametime / RES_COUNT;
	for (unsigned i = 0; i < RES_COUNT - 1; i++) {
	    moveball(restime);
	}
	// Account for rounding errors
	moveball(frametime - restime * (RES_COUNT - 1));

	if (iswincondition()) {
	    aud_sound_play(AUD_WIN);
	    level++;
	    if (level > LVL_COUNT) {
		level = 1;
	    }
	    resetlevel();
	}
    }
}

void leveldraw(void) {
    for (unsigned i = 0; i < BRICK_COLS * BRICK_ROWS; i++) {
        if (bricks[i].isactive && !bricks[i].isdestroyed) {
	    gfx_render_quad(&render_quad, &bricks[i].quad);
        }
    }
}

void game_render(void) {
    gfx_render_begin(&render_bg);
    gfx_render_quad(&render_bg, &bg);
    gfx_render_end(&render_bg);

    gfx_render_begin(&render_quad);
    leveldraw();
    gfx_render_quad(&render_quad, &ball.quad);
    gfx_render_quad(&render_quad, &paddle);
    gfx_render_end(&render_quad);
}
