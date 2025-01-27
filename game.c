/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 *
 * TODO: Ball can get stuck in corner.
 */

#define CGLM_PRINT_COLOR       ""
#define CGLM_PRINT_COLOR_RESET ""
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

enum { SoundBrick, SoundDeath, SoundClear, SoundWon, SoundLost, SoundCount };
enum { StateLoading, StateMenu, StateRun, StatePause, StateWon, StateLost } state = StateLoading;
enum FontSize { FontLarge, FontMedium, FontSizeCount };
typedef enum FontSize FontSize;

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
    Sprite sprite;
} Ball;

typedef struct {
    Sprite sprite;
} Paddle;

typedef struct {
    bool     is_active;
    bool     is_solid;
    bool     is_double;
    bool     is_first; // First of a double pair
    bool     is_destroyed;
    Sprite   sprite;
} Brick;

typedef struct {
    Ball     ball;
    Paddle   paddle;
    Brick*   bricks;
    Renderer render;
} Sprites;

typedef struct {
    FontSize size;
    vec2s    pos;
    vec3s    col;
    char*    fmt;
} Text;

// Function prototypes

static void paddle_init(Sprite* ps);
static void ball_init(Ball* b, Sprite* bs, Sprite* p);
static void brick_init(Brick* brick, char id, unsigned col, unsigned row);
static void level_read(const char* data);
static void level_load(unsigned num);
static void level_unload(void);
static void level_reset(void);
static void quit(void);
static void pause(void);
static unsigned random(unsigned min, unsigned max);
static void click(void);
static vec2s get_wall_dist(Sprite* s);
static void bounce(Ball* b, Sprite* bs, vec2s vel, vec2s dist, bool is_paddle);
static bool is_wall_hit(Sprite* s, vec2s newpos);
static bool is_oob(Sprite* s, vec2s newpos);
static bool is_paddle_hit(Sprite* bs, Sprite* ps, vec2s newpos);
static vec2s get_aabb_dist(Sprite* s1, Sprite* s2);
static unsigned get_brick_index(float x, float y);
static unsigned get_brick_hits(Sprite* bs, vec2s newpos, unsigned brick_hits[VERT_COUNT]);
static void score_update(unsigned brick_index);
static vec2s get_brick_closest(Sprite* s, unsigned count, unsigned brick_hits[VERT_COUNT]);
static void ball_move(Ball* b, Sprite* s, double frame_time);
static bool is_won(void);
static void level_render(void);
static void screen_render(Screen* s);
static void text_render(const Text* t);
static void screen_game(void);

// Requires above types and functions
#include "config.h"

// Constants
static const unsigned SPRITE_COUNT = 200;

// Variables
static Screen loading, bg;
static Sprites sprites;
static unsigned level = 1;
static ma_sound** sounds;
static ma_sound** music;
static ma_sound* playing;
static Font fonts[FontSizeCount];
static unsigned score = 0, hiscore, lives = LIVES;
static bool is_hiscore = false;

// Function implementations

#ifndef NDEBUG

// ASCII art breakout anyone?
void brick_print(unsigned count, unsigned brick_hits[VERT_COUNT]) {
    for (unsigned row = 0; row < BRICK_ROWS; row++) {
	for (unsigned col = 0; col < BRICK_COLS; col++) {
	    unsigned i = row * BRICK_COLS + col;

	    bool hit = false;
	    for (unsigned j = 0; j < count; j++) {
		if (brick_hits[j] == i) {
		    fputc('!', stderr);
		    fputc(' ', stderr);
		    hit = true;
		    break;
		}
	    }
	    if (hit) continue;

	    Brick* b = &sprites.bricks[i];
	    char c;
	    if (b->is_active) {
		if (b->is_destroyed) {
		    c = 'x';
		} else {
		    c = 'o';
		};
	    } else {
		c = '_';
	    }
	    fputc(c, stderr);
	    fputc(' ', stderr);
	}
	fputc('\n', stderr);
    }
    fputc('\n', stderr);
}

#endif

void paddle_init(Sprite* ps) {
    vec2s mouse_pos = main_get_mouse_pos();
    vec2s pos = {{
	mouse_pos.x,
	SCR_HEIGHT - PADDLE_SIZE.t
    }};
    ps->quad = gfx_quad_create(&sprites.render, pos, PADDLE_SIZE, PADDLE_OFFSET);
    ps->size = PADDLE_SIZE;
}

void ball_init(Ball* b, Sprite* bs, Sprite* ps) {
    b->is_stuck = true;
    vec2s pos = {{
        ps->pos.x + ps->size.s / 2.0f - BALL_SIZE.s / 2.0f,
        ps->pos.y - BALL_SIZE.t
    }};
    bs->quad = gfx_quad_create(&sprites.render, pos, BALL_SIZE, BALL_OFFSET);
    bs->size = BALL_SIZE;
}

void brick_init(Brick* b, char id, unsigned col, unsigned row) {
    bool is_double = isupper(id);
    bool is_solid = !(is_double || isdigit(id));
    bool is_first = false;
    if (is_double) {
	if (col == 0) {
	    is_first = true;
	} else {
	    Brick *b_prev = &sprites.bricks[col - 1 + row * BRICK_COLS];
	    is_first = !(b_prev->is_active && b_prev->is_double && b_prev->is_first);
	}
    }

    b->is_active    = true;
    b->is_solid     = is_solid;
    b->is_double    = is_double;
    b->is_first     = is_first;
    b->is_destroyed = false;

    vec2s pos = {{
        BG_WALL_LEFT + col * BRICK_SIZE.s,
        BG_WALL_TOP  + row * BRICK_SIZE.t
    }};

    float width = is_double ? BRICK_SIZE.s * 2 : BRICK_SIZE.s;
    vec2s size = (vec2s) {{ width, BRICK_SIZE.t }};

    vec2s offset;
    if      (is_double) offset = BRICK_DOUBLE_OFFSETS[id - 'A'];
    else if (is_solid)  offset = BRICK_SOLID_OFFSETS[ id - 'a'];
    else                offset = BRICK_SINGLE_OFFSETS[id - '0'];

    b->sprite.quad = gfx_quad_create(&sprites.render, pos, size, offset);
    b->sprite.size = size;
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
        } else if (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
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
    char* data = util_load(file, READ_ONLY_TEXT);
    if (!data) main_term(EXIT_FAILURE, "Unable to load level:\n%s\n", level);

    sprites.bricks = (Brick*) malloc(BRICK_COLS * BRICK_ROWS * sizeof(Brick));
    level_read(data);

    util_unload(data);
}

void screen_init(Screen* s, const char* file) {
    vec2s pos  = (vec2s) {{ 0, 0 }};
    vec2s size = (vec2s) {{ SCR_WIDTH, SCR_HEIGHT }};
    s->render  = gfx_render_create(1, file);
    s->quad    = gfx_quad_create(&loading.render, pos, size, pos);
}

// Do the minimum required to get a loading screen
void game_loading(void) {
    gfx_init();
    screen_init(&loading, LOADING_FILE);
}

unsigned hiscore_load(void) {
    char* data = util_load(HISCORE_FILE, READ_ONLY_TEXT);
    if (data) {
	char* end = NULL;
	return strtoul(data, &end, 10);
    } else {
	return 0;
    }
}

void game_load(void) {
    // Decent random seed: https://stackoverflow.com/q/58150771
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    srand(ts.tv_nsec);

    screen_init(&bg, BG_FILE);

    sprites.render = gfx_render_create(SPRITE_COUNT, SPRITE_SHEET);
    paddle_init(&sprites.paddle.sprite);
    ball_init(&sprites.ball, &sprites.ball.sprite, &sprites.paddle.sprite);
    level_load(level);

    for (unsigned i = 0; i < COUNT(FONT_HEIGHTS); i++) {
	fonts[i] = gfx_font_create(FONT_HEIGHTS[i], FONT_FILE);
    }

    aud_init(AUD_VOL);
    sounds = (ma_sound**) malloc(SoundCount * sizeof(ma_sound*));
    sounds[SoundBrick] = aud_sound_load(AUD_BRICK);
    sounds[SoundDeath] = aud_sound_load(AUD_DEATH);
    sounds[SoundClear] = aud_sound_load(AUD_CLEAR);
    sounds[SoundWon]   = aud_sound_load(AUD_WON);
    sounds[SoundLost]  = aud_sound_load(AUD_LOST);
    playing            = aud_sound_load(AUD_MUSIC[level - 1]);

    hiscore = hiscore_load();

    state = StateMenu;
}

void level_unload(void) {
    if (sprites.bricks) free(sprites.bricks);
}

void level_fullreset(void) {
    score = 0;
    lives = LIVES;
    is_hiscore = 0;
    level_reset();
}

void level_reset(void) {
    level_unload();

    ball_init(&sprites.ball, &sprites.ball.sprite, &sprites.paddle.sprite);
    level_load(level);
}

void music_unload(void) {
    if (!music) return;

    for (size_t i = 0; i < COUNT(AUD_MUSIC); i ++) {
	aud_sound_unload(music[i]);
    }
    free(music);
}

void game_unload(void) {
    music_unload();
    if (sounds) {
	for (unsigned i = 0; i < SoundCount; i++) {
	    if (sounds[i]) aud_sound_unload(sounds[i]);
	}
	free(sounds);
    }
    aud_term();

    for (unsigned i = 0; i < FontSizeCount; i++) {
	gfx_font_delete(&fonts[i]);
    }

    level_unload();
    gfx_render_delete(&sprites.render);
    gfx_render_delete(&loading.render);
    gfx_term();
}

void quit(void) {
    switch(state) {
    case StateLoading:
	break;
    case StateMenu:
	main_quit();
	break;
    case StateRun:
    case StatePause:
    case StateWon:
    case StateLost:
	level_fullreset();
	state = StateMenu;
	break;
    }
}

void pause(void) {
    switch(state) {
    case StateLoading:
    case StateMenu:
    case StateWon:
    case StateLost:
	break;
    case StatePause:
	state = StateRun;
	break;
    case StateRun:
	state = StatePause;
	break;
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

void click(void) {
    switch (state) {
    case StateLoading:
	break;
    case StatePause:
	state = StateRun;
	break;
    case StateWon:
    case StateLost:
	level_fullreset();
	state = StateMenu;
	break;
    case StateMenu:
	state = StateRun;
	break;
    case StateRun:
	Ball* b = &sprites.ball;

	if (b->is_stuck) {
	    b->vel = BALL_RELEASE[random(0, COUNT(BALL_RELEASE) - 1)];
	    b->vel = glms_vec2_normalize(b->vel);

	    b->is_stuck = false;
	}
	break;
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
    case StateLoading:
    case StateMenu:
    case StatePause:
    case StateWon:
    case StateLost:
	break;
    case StateRun:
	Sprite* ps = &sprites.paddle.sprite;
	Sprite* bs = &sprites.ball.sprite;

	vec2s mouse_pos = main_get_mouse_pos();
	mouse_pos.x = CLAMP(
		mouse_pos.x,
		BG_WALL_LEFT,
		SCR_WIDTH - ps->size.s - BG_WALL_RIGHT
	);
	vec2s newpos = (vec2s) {{
	    mouse_pos.x,
	    ps->pos.y
	}};
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
    }
}

vec2s get_wall_dist(Sprite* s) {
    float left  = s->pos.x - BG_WALL_LEFT;
    float right = SCR_WIDTH - BG_WALL_RIGHT - s->size.s - s->pos.x;
    float top   = s->pos.y - BG_WALL_TOP;
    return (vec2s) {{
	left < right ? left : right,
	top
    }};
}

void bounce(Ball* b, Sprite* bs, vec2s vel, vec2s dist, bool is_paddle) {
    // Calculate which axis will hit first
    float time_x   = vel.x != 0.0f ? fabs(dist.s / vel.x) : 0.0f;
    float time_y   = vel.y != 0.0f ? fabs(dist.t / vel.y) : 0.0f;
    float time_min = fmin(time_x, time_y);

    // Move ball to the point of the collision
    vel = glms_vec2_scale(vel, time_min);
    gfx_quad_add_vec(&bs->quad, vel);

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
    return newpos.x < BG_WALL_LEFT ||
           newpos.x > SCR_WIDTH - BG_WALL_RIGHT - s->size.s ||
           newpos.y < BG_WALL_TOP;
}

bool is_oob(Sprite* s, vec2s newpos) {
    return newpos.y + s->size.x > SCR_HEIGHT;
}

bool is_paddle_hit(Sprite* bs, Sprite* ps, vec2s newpos) {
    return newpos.y > SCR_HEIGHT - ps->size.y - bs->size.y &&
           newpos.y < SCR_HEIGHT &&
           newpos.x < ps->pos.x + ps->size.x &&
           newpos.x + bs->size.x > ps->pos.x;
}

vec2s get_aabb_dist(Sprite* s1, Sprite* s2) {
    vec2s dist = {};
    float s1x = s1->pos.x;
    float s1y = s1->pos.y;
    float s2x = s2->pos.x;
    float s2y = s2->pos.y;

    if (s1x < s2x) {
        dist.s = s2x - (s1x + s1->size.s);
    } else if (s1x > s2x) {
        dist.s = s1x - (s2x + s2->size.s);
    } else {
	dist.s = 0.0f;
    }
    if (s1y < s2y) {
        dist.t = s2y - (s1y + s1->size.t);
    } else if (s1y > s2y) {
        dist.t = s1y - (s2y + s2->size.t);
    } else {
	dist.t = 0.0f;
    }

    return dist;
}

// Bricks are in a grid and don't move so we can get the index from a position
unsigned get_brick_index(float x, float y) {
    unsigned col = (x - BG_WALL_LEFT) / BRICK_SIZE.s;
    unsigned row = (y - BG_WALL_TOP) / BRICK_SIZE.t;
    return col + row * BRICK_COLS;
}

// Check if any of the four corners of the ball hit a brick
unsigned get_brick_hits(Sprite* s, vec2s newpos, unsigned brick_hits[VERT_COUNT]) {
    unsigned indices[VERT_COUNT];
    unsigned count = 0;

    // Is the ball within the brick area?
    if (newpos.y < BG_WALL_TOP + BRICK_ROWS * BRICK_SIZE.t) {
	// Get the brick index for each corner of the ball
        indices[0] = get_brick_index(newpos.x, newpos.y);
        indices[1] = get_brick_index(newpos.x + s->size.s, newpos.y);
        indices[2] = get_brick_index(newpos.x, newpos.y + s->size.t);
        indices[3] = get_brick_index(newpos.x + s->size.s, newpos.y + s->size.t);
        for (unsigned i = 0; i < VERT_COUNT; i++) {
            unsigned j = indices[i];
	    // Corners of ball may evaluate to indices outside ball area
            if (j < BRICK_ROWS * BRICK_COLS) {
		Brick* b = &sprites.bricks[j];
		if (b->is_active && !b->is_destroyed) {
		    brick_hits[count++] = j;
		}
            }
        }
    }

    return count;
}

void score_update(unsigned brick_index) {
    Brick* b = &sprites.bricks[brick_index];

    unsigned score_base = b->is_double ? 2 : 1;
    unsigned row = brick_index / BRICK_COLS;

    score += score_base * level * (BRICK_ROWS - row);
}

// Find the distance to the closest brick and destroy it, if applicable
vec2s get_brick_closest(Sprite* s, unsigned count, unsigned brick_hits[VERT_COUNT]) {
    float closest = FLT_MAX;
    unsigned brick = 0;
    vec2s dist_min = {};

    for (unsigned i = 0; i < count; i++) {
	Sprite* brick_sprite = &sprites.bricks[brick_hits[i]].sprite;
        vec2s dist = get_aabb_dist(s, brick_sprite);
        if (dist.s < closest && dist.s < dist.t) {
            dist_min = dist;
            closest  = dist.s;
            brick    = i;
        } else if (dist.t < closest) {
            dist_min = dist;
            closest  = dist.t;
            brick    = i;
        }
    }

    unsigned i = brick_hits[brick];
    Brick* b = &sprites.bricks[i];
    if (!b->is_solid) {
        b->is_destroyed = true;
	if (b->is_double) {
	    if (b->is_first) {
		sprites.bricks[i + 1].is_destroyed = true;
	    } else {
		sprites.bricks[i - 1].is_destroyed = true;
	    }
	}
	score_update(i);
        aud_sound_play(AUD_BRICK);
    }

    return dist_min;
}

void ball_move(Ball* b, Sprite* bs, double frame_time) {
    Sprite* ps   = &sprites.paddle.sprite;
    vec2s vel    = glms_vec2_scale(b->vel, BALL_SPEED * frame_time);
    vec2s newpos = glms_vec2_add(bs->pos, vel);

    if (is_oob(bs, newpos)) {
	lives -= 1;
	if (!lives) {
	    state = StateLost;
	    aud_sound_stop(playing);
	    aud_sound_play(AUD_LOST);
	} else {
	    level_reset();
	    aud_sound_play(AUD_DEATH);
	}
    } else if (is_wall_hit(bs, newpos)) {
        vec2s dist = get_wall_dist(bs);
        bounce(b, bs, vel, dist, false);
    } else if (is_paddle_hit(bs, ps, newpos)) {
        vec2s dist = get_aabb_dist(bs, ps);
        bounce(b, bs, vel, dist, true);
    } else {
        unsigned brick_hits[VERT_COUNT];
        unsigned count = get_brick_hits(bs, newpos, brick_hits);
        if (count > 0) {
#ifndef NDEBUG
	    //brick_print(count, brick_hits);
#endif
            vec2s dist = get_brick_closest(bs, count, brick_hits);
            bounce(b, bs, vel, dist, false);
        } else {
	    gfx_quad_set_pos(&bs->quad, newpos, bs->size);
        }
    }
}

bool is_won(void) {
    for (unsigned i = 0; i < BRICK_COLS * BRICK_ROWS; i++) {
	Brick* b = &sprites.bricks[i];
        if (b->is_active && !b->is_solid && !b->is_destroyed) return false;
    }

    return true;
}

void game_update(double frame_time) {
    switch(state) {
    case StateLoading:
    case StateMenu:
    case StatePause:
    case StateWon:
    case StateLost:
	break;
    case StateRun:
	if (!sprites.ball.is_stuck) {
	    Ball* b = &sprites.ball;

	    double cr_time = frame_time / CR_COUNT;
	    for (unsigned i = 0; i < CR_COUNT - 1; i++) {
		ball_move(b, &b->sprite, cr_time);
	    }
	    // Account for rounding errors
	    ball_move(b, &b->sprite, frame_time - cr_time * (CR_COUNT - 1));

	    if (is_won()) {
		level++;
		if (level > LEVEL_COUNT) {
		    state = StateWon;
		    aud_sound_stop(playing);
		    aud_sound_play(AUD_WON);
		    if (score > hiscore) {
			hiscore = score;
			is_hiscore = true;
		    }
		    level = 1;
		} else {
		    aud_sound_stop(playing);
		    aud_sound_play(AUD_CLEAR);
		    level_reset();
		}
	    }
	}

	if (state == StateRun && !ma_sound_is_playing(playing)) {
	    playing = aud_sound_play(AUD_MUSIC[level - 1]);
	}
    }
}

void level_render(void) {
    for (unsigned i = 0; i < BRICK_COLS * BRICK_ROWS; i++) {
	Brick* b = &sprites.bricks[i];
        if (b->is_active && !b->is_destroyed && (!b->is_double || b->is_first)) {
            gfx_render_quad(&sprites.render, &b->sprite.quad);
        }
    }
}

void screen_render(Screen* s) {
    gfx_render_begin(&s->render);
    gfx_render_quad(&s->render, &s->quad);
    gfx_render_end(&s->render);
}

void text_render(const Text* t) {
    gfx_font_begin(&fonts[t->size]);
    gfx_font_printf(&fonts[t->size], t->pos, t->col, t->fmt, score);
    gfx_font_end(&fonts[t->size]);
}

void screen_game(void) {
    screen_render(&bg);

    gfx_render_begin(&sprites.render);
    level_render();
    gfx_render_quad(&sprites.render, &sprites.paddle.sprite.quad);
    gfx_render_quad(&sprites.render, &sprites.ball.sprite.quad);
    gfx_render_end(&sprites.render);

    text_render(&TEXT_SCORE);
}

void game_render(void) {
    switch(state) {
    case StateLoading:
	screen_render(&loading);
	break;
    case StateMenu:
	screen_render(&loading);
	text_render(&TEXT_MENU);
	break;
    case StatePause:
	screen_game();
	text_render(&TEXT_PAUSED);
	break;
    case StateRun:
	screen_game();
	break;
    case StateWon:
	screen_game();
	text_render(&TEXT_WON);
	if (is_hiscore) text_render(&TEXT_HISCORE);
	text_render(&TEXT_CONTINUE);
	break;
    case StateLost:
	screen_game();
	text_render(&TEXT_LOST);
	if (is_hiscore) text_render(&TEXT_HISCORE);
	text_render(&TEXT_CONTINUE);
	break;
    }
}
