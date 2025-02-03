/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 *
 */

#define CGLM_PRINT_COLOR       ""
#define CGLM_PRINT_COLOR_RESET ""
#define GLFW_INCLUDE_NONE
#include <assert.h>
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
    Sprite   sprite;
    unsigned score;
    unsigned lives;
    Sprite*  lives_sprites;
} Paddle;

typedef struct {
    bool     is_active;
    bool     is_solid;
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

static void paddle_init(Paddle* p);
static void ball_init(Ball* b, Sprite* bs, Sprite* p);
static void brick_init(Brick* brick, char id, unsigned col, unsigned row);
static void level_read(const char* data);
static void level_load(unsigned num);
static void screen_init(Screen* s, const char* file);
static unsigned hiscore_load(void);
static void level_unload(void);
static void level_reset(void);
static void hiscore_save(void);
#ifndef NDEBUG
static void level_next(void);
static void level_prev(void);
#endif // !NDEBUG
static void quit(void);
static void pause(void);
static unsigned random(unsigned min, unsigned max);
static void click(void);
static bool is_oob(Sprite* bs, vec2s newpos);
static bool is_wall_hit(Sprite* bs, vec2s newpos);
static vec2s get_wall_dist(Ball* b, Sprite* bs);
static void bounce(Ball* b, Sprite* bs, Sprite* ps, vec2s vel, vec2s dist, bool is_paddle);
static void hiscore_check(void);
static bool is_paddle_hit(Sprite* bs, Sprite* ps, vec2s newpos);
static vec2s get_aabb_dist(Ball* b, Sprite* bs, Sprite* s);
static int get_brick_index(float x, float y);
static bool is_member(unsigned index, unsigned brick_hits[VERT_COUNT]);
static unsigned get_brick_hits(Sprite* bs, vec2s newpos, unsigned brick_hits[VERT_COUNT]);
static void score_update(unsigned brick_index);
static vec2s get_brick_closest(Ball* b, Sprite* bs, unsigned count, unsigned brick_hits[VERT_COUNT]);
static void ball_move(Ball* b, Sprite* bs, double frame_time);
static bool is_won(void);
static void level_render(void);
static void screen_render(Screen* s);
static void text_render(const Text* t, ...);
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
static unsigned hiscore;
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

void paddle_init(Paddle* p) {
    p->score = 0;
    p->lives = LIVES;

    vec2s mouse_pos = main_get_mouse_pos();
    vec2s pos = {{
	mouse_pos.x,
	SCR_HEIGHT - PADDLE_SIZE.t
    }};
    p->sprite.quad = gfx_quad_create(&sprites.render, pos, PADDLE_SIZE, PADDLE_OFFSET);
    p->sprite.size = PADDLE_SIZE;

    p->lives_sprites = (Sprite*) malloc((LIVES - 1) * sizeof(Sprite));
    for (size_t i = 0; i < LIVES - 1; i++) {
	p->lives_sprites[i].quad = gfx_quad_create(&sprites.render, PADDLE_LIVES[i], PADDLE_SIZE, PADDLE_OFFSET);
	p->lives_sprites[i].size = PADDLE_SIZE;
    }
}

void ball_init(Ball* b, Sprite* bs, [[maybe_unused]] Sprite* ps) {
    b->is_stuck = true;
    vec2s pos = {{
        ps->pos.x + ps->size.s / 2.0f - BALL_SIZE.s / 2.0f,
        ps->pos.y - BALL_SIZE.t
    }};
    bs->quad = gfx_quad_create(&sprites.render, pos, BALL_SIZE, BALL_OFFSET);
    bs->size = BALL_SIZE;
}

void brick_init(Brick* b, char id, unsigned col, unsigned row) {
    bool is_solid = !isdigit(id);

    b->is_active    = true;
    b->is_solid     = is_solid;
    b->is_destroyed = false;

    vec2s pos = {{
        BG_WALL_LEFT + col * BRICK_SIZE.s,
        BG_WALL_TOP  + row * BRICK_SIZE.t
    }};

    vec2s offset;
    if (is_solid) offset = BRICK_SOLID_OFFSETS[ id - 'a'];
    else          offset = BRICK_SINGLE_OFFSETS[id - '0'];

    b->sprite.quad = gfx_quad_create(&sprites.render, pos, BRICK_SIZE, offset);
    b->sprite.size = BRICK_SIZE;
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
    int size = snprintf(NULL, 0, fmt, LEVEL_FOLDER, num);
    char file[size + 1];
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
    FILE* fp = fopen(HISCORE_FILE, READ_ONLY_TEXT);
    if (!fp) {
        fprintf(stderr, "Could not open file %s\n", HISCORE_FILE);
        perror("fopen() error");
        return 0;
    }

    unsigned hiscore;
    if (fscanf(fp, "%u", &hiscore) != 1) {
	fprintf(stderr, "Could not read hiscore from file %s\n", HISCORE_FILE);
        return 0;
    }

    if (fclose(fp) == EOF) {
        fprintf(stderr, "Error on closing file %s\n", HISCORE_FILE);
        perror("fclose() error");
    }

    return hiscore;
}

void music_load(void) {
    music = (ma_sound**) malloc(COUNT(AUD_MUSIC) * sizeof(ma_sound*));
    for (size_t i = 0; i < COUNT(AUD_MUSIC); i ++) {
	music[i] = aud_sound_load(AUD_MUSIC[i]);
    }
}

void game_load(void) {
    // Decent random seed: https://stackoverflow.com/q/58150771
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    srand(ts.tv_nsec);

    screen_init(&bg, BG_FILE);

    sprites.render = gfx_render_create(SPRITE_COUNT, SPRITE_SHEET);
    paddle_init(&sprites.paddle);
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
    music_load();

    hiscore = hiscore_load();
}

// Don't go to the menu till main indicates to
void game_loaded(void) {
    state = StateMenu;
}

void level_unload(void) {
    if (sprites.bricks) free(sprites.bricks);
}

void level_fullreset(void) {
    sprites.paddle.score = 0;
    sprites.paddle.lives = LIVES;
    is_hiscore = false;
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
    if (sprites.paddle.lives_sprites) free(sprites.paddle.lives_sprites);
    gfx_render_delete(&sprites.render);
    gfx_render_delete(&loading.render);
    gfx_term();
}

void hiscore_save(void) {
    FILE* fp = fopen(HISCORE_FILE, WRITE_ONLY_TEXT);
    if (!fp) {
        fprintf(stderr, "Could not open file %s.\n", HISCORE_FILE);
	perror("fopen() failed");
    }

    if (fprintf(fp, "%u\n", hiscore) < 0) {
	perror("fprintf failed");
    }

    fclose(fp);
}

#ifndef NDEBUG

void level_next(void) {
    if (level < LEVEL_COUNT) {
	level++;
	aud_sound_stop(playing);
	level_reset();
    }
}

void level_prev(void) {
    if (level > 1) {
	level--;
	aud_sound_stop(playing);
	level_reset();
    }
}

#endif // !NDEBUG

void quit(void) {
    switch(state) {
    case StateLoading:
	break;
    case StateMenu:
	hiscore_save();
	main_quit();
	break;
    case StateRun:
    case StatePause:
	aud_sound_stop(playing);
	[[fallthrough]];
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
[[maybe_unused]] unsigned random(unsigned min, unsigned max) {
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

bool is_ball_hit(Sprite* bs, Sprite* ps, vec2s newpos) {
    assert(bs);
    assert(ps);
    assert(newpos.x >= BG_WALL_LEFT);
    assert(newpos.x <= SCR_WIDTH - BG_WALL_RIGHT - PADDLE_SIZE.s);
    assert(newpos.y == SCR_HEIGHT - PADDLE_SIZE.t);

    return bs->pos.y > SCR_HEIGHT - ps->size.t - bs->size.t &&
           bs->pos.x < newpos.x + ps->size.s &&
           bs->pos.x + bs->size.s > newpos.x;
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

	// Refuse to move the paddle into the ball
	if (is_ball_hit(bs, ps, newpos)) {
	    main_set_mouse_pos(ps->pos);
	    return;
	}

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
    }
}

bool is_oob(Sprite* bs, vec2s newpos) {
    assert(bs);
    assert(newpos.x > BG_WALL_LEFT - 10.0f);
    assert(newpos.x < SCR_WIDTH - BG_WALL_RIGHT - BALL_SIZE.s + 10.0f);
    assert(newpos.y > BG_WALL_TOP - 10.0f);
    assert(newpos.y < SCR_HEIGHT - BALL_SIZE.s + 10.0f);

    return newpos.y + bs->size.x > SCR_HEIGHT;
}

bool is_wall_hit(Sprite* bs, vec2s newpos) {
    assert(bs);
    assert(newpos.x > BG_WALL_LEFT - 10.0f);
    assert(newpos.x < SCR_WIDTH - BG_WALL_RIGHT - BALL_SIZE.s + 10.0f);
    assert(newpos.y > BG_WALL_TOP - 10.0f);
    assert(newpos.y < SCR_HEIGHT - BALL_SIZE.s + 10.0f);

    return newpos.x < BG_WALL_LEFT ||
           newpos.x > SCR_WIDTH - BG_WALL_RIGHT - bs->size.s ||
           newpos.y < BG_WALL_TOP;
}

vec2s get_wall_dist(Ball* b, Sprite* bs) {
    assert(b);
    assert(bs);

    float s;
    if (signbit(b->vel.x)) {
	s = bs->pos.x - BG_WALL_LEFT;

	assert(s >= 0.0f);
	assert(s <= SCR_WIDTH - BG_WALL_LEFT);
    } else {
	s = SCR_WIDTH - BG_WALL_RIGHT - bs->size.s - bs->pos.x;

	assert(s >= 0.0f);
	assert(s <= SCR_WIDTH - BG_WALL_RIGHT - BALL_SIZE.s);
    }

    float t;
    if (signbit(b->vel.y)) {
	t = bs->pos.y - BG_WALL_TOP;

	assert(t >= 0.0f);
	assert(t <= SCR_HEIGHT - BG_WALL_TOP);
    } else {
	t = FLT_MAX;
    }

    return (vec2s) {{ s, t }};
}

void bounce(Ball* b, Sprite* bs, Sprite* ps, vec2s vel, vec2s dist, bool is_paddle) {
    assert(b);
    assert(bs);

    // Find which axis will hit first.
    // Ignore negative distances as the ball can't hit on that axis.
    float time_x = FLT_MAX;
    if (!signbit(dist.s)) time_x = vel.x == 0.0f ? 0.0f : dist.s / fabs(vel.x);
    float time_y = FLT_MAX;
    if (!signbit(dist.t)) time_y = vel.y == 0.0f ? 0.0f : dist.t / fabs(vel.y);
    float time_min = fmin(time_x, time_y);

    assert(time_x >= 0.0f);
    assert(time_y >= 0.0f);
    assert(time_min >= 0.0f);
    assert(time_min < 1.0f);

    // Move ball to the point of collision
    vel = glms_vec2_scale(vel, time_min);
    gfx_quad_add_vec(&bs->quad, vel);
    // Make sure ball isn't inisde a wall
    vec2s newpos = {{
	CLAMP(bs->pos.x, BG_WALL_LEFT, SCR_WIDTH - BG_WALL_RIGHT - bs->size.s),
	MAX(  bs->pos.y, BG_WALL_TOP)
    }};
    gfx_quad_set_pos(&bs->quad, newpos, bs->size);
#ifndef NDEBUG
    glms_vec2_print(newpos, stderr);
#endif

    assert(bs->pos.x >= BG_WALL_LEFT);
    assert(bs->pos.x <= SCR_WIDTH - BG_WALL_RIGHT - BALL_SIZE.s);
    assert(bs->pos.y >= BG_WALL_TOP);
    assert(bs->pos.y <= SCR_HEIGHT - BALL_SIZE.t);

    // Finally, bounce the ball
    if (time_x == time_y) {
        b->vel.x = -b->vel.x;
        b->vel.y = -b->vel.y;
    } else if (time_x < time_y) {
        b->vel.x = -b->vel.x;
    } else {
	// Adjust horizontal velocity based on distance from the paddles centre
	if (is_paddle) {
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

bool is_paddle_hit(Sprite* bs, Sprite* ps, vec2s newpos) {
    assert(bs);
    assert(ps);
    assert(newpos.x > BG_WALL_LEFT - 10.0f);
    assert(newpos.x < SCR_WIDTH - BG_WALL_RIGHT - BALL_SIZE.s + 10.0f);
    assert(newpos.y > BG_WALL_TOP - 10.0f);
    assert(newpos.y < SCR_HEIGHT - BALL_SIZE.s + 10.0f);

    return newpos.y > SCR_HEIGHT - ps->size.t - bs->size.t &&
           newpos.x < ps->pos.x + ps->size.s &&
           newpos.x + bs->size.s > ps->pos.x;
}

vec2s get_aabb_dist(Ball* b, Sprite* bs, Sprite* s2) {
    assert(b);
    assert(bs);
    assert(s2);

    float bsx = bs->pos.x;
    float bsy = bs->pos.y;
    float s2x = s2->pos.x;
    float s2y = s2->pos.y;

    assert(bsx >= BG_WALL_LEFT);
    assert(bsx <= SCR_WIDTH - BG_WALL_LEFT);
    assert(bsy >= BG_WALL_TOP);
    assert(bsy <= SCR_HEIGHT);

    float s = signbit(b->vel.x) ? bsx - (s2x + s2->size.s) : s2x - (bsx + bs->size.s);

    assert(s >= -((float) SCR_WIDTH - BG_WALL_LEFT - BG_WALL_RIGHT));
    assert(s <=           SCR_WIDTH - BG_WALL_LEFT - BG_WALL_RIGHT);

    float t = signbit(b->vel.y) ? bsy - (s2y + s2->size.t) : s2y - (bsy + bs->size.t);

    assert(t >= -((float) SCR_HEIGHT - BG_WALL_TOP));
    assert(t <=           SCR_HEIGHT - BG_WALL_TOP);

    return (vec2s) {{ s, t }};
}

// Bricks are in a grid and don't move so we can get the index from a position
int get_brick_index(float x, float y) {
    assert(x > BG_WALL_LEFT - 10.0f);
    assert(x < SCR_WIDTH - BG_WALL_RIGHT + 10.0f);
    assert(y > BG_WALL_TOP - 10.0f);
    assert(y < SCR_HEIGHT + 10.0f);

    if (x > BG_WALL_LEFT && x < SCR_WIDTH - BG_WALL_RIGHT &&
	y > BG_WALL_TOP  && y < BG_WALL_TOP + BRICK_ROWS * BRICK_SIZE.t) {
	unsigned col = (x - BG_WALL_LEFT) / BRICK_SIZE.s;
	unsigned row = (y - BG_WALL_TOP)  / BRICK_SIZE.t;
	signed i = col + row * BRICK_COLS;
	assert(i < (signed) (BRICK_COLS * BRICK_ROWS));

	Brick* b = &sprites.bricks[i];
	assert(b);
	if (b->is_active && (b->is_solid || !b->is_destroyed)) return i;
    }

    return -1;
}

bool is_member(unsigned index, unsigned brick_hits[VERT_COUNT]) {
    for (unsigned i = 0; i < VERT_COUNT; i++) {
	if (brick_hits[i] == index) return true;
    }

    return false;
}

// Check if any of the four corners of the ball hit a brick
unsigned get_brick_hits(Sprite* s, vec2s newpos, unsigned brick_hits[VERT_COUNT]) {
    assert(s);
    assert(newpos.x > BG_WALL_LEFT - 10.0f);
    assert(newpos.x < SCR_WIDTH - BG_WALL_RIGHT - BALL_SIZE.s + 10.0f);
    assert(newpos.y > BG_WALL_TOP - 10.0f);
    assert(newpos.y < SCR_HEIGHT - BALL_SIZE.s + 10.0f);
    static_assert(VERT_COUNT == 4);

    unsigned count = 0;

    // Is the ball within the brick area?
    if (newpos.y < BG_WALL_TOP + BRICK_ROWS * BRICK_SIZE.t) {
	// Get the brick index for each corner of the ball but check if oob
	int i = get_brick_index(newpos.x, newpos.y);
	if (i > -1 && !is_member(i, brick_hits)) brick_hits[count++] = i;
	i = get_brick_index(newpos.x + s->size.s, newpos.y);
	if (i > -1 && !is_member(i, brick_hits)) brick_hits[count++] = i;
	i = get_brick_index(newpos.x, newpos.y + s->size.t);
	if (i > -1 && !is_member(i, brick_hits)) brick_hits[count++] = i;
	i = get_brick_index(newpos.x + s->size.s, newpos.y + s->size.t);
	if (i > -1 && !is_member(i, brick_hits)) brick_hits[count++] = i;
    }

    assert(count < VERT_COUNT);
    return count;
}

void score_update(unsigned brick_index) {
    unsigned score_base = 1;
    unsigned row = brick_index / BRICK_COLS;

    sprites.paddle.score += score_base * level * (BRICK_ROWS - row);
}

// Find the distance to the closest brick and destroy it, if applicable
vec2s get_brick_closest(Ball* b, Sprite* bs, unsigned count, unsigned brick_hits[VERT_COUNT]) {
    assert(b);
    assert(bs);
    assert(count > 0);
    assert(count <= VERT_COUNT);

    float closest = FLT_MAX;
    unsigned brick = 0;
    vec2s dist_min = {};

    for (unsigned i = 0; i < count; i++) {
	Sprite* brick_sprite = &sprites.bricks[brick_hits[i]].sprite;
	assert(brick_sprite);
	vec2s dist = get_aabb_dist(b, bs, brick_sprite);
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
    Brick* bp = &sprites.bricks[i];
    assert(bp);
    assert(bp->is_active);
    assert(!bp->is_destroyed);
    if (!bp->is_solid) {
        bp->is_destroyed = true;
	score_update(i);
        aud_sound_play(AUD_BRICK);
    }

    return dist_min;
}

void hiscore_check(void) {
    if (sprites.paddle.score > hiscore) {
	hiscore = sprites.paddle.score;
	is_hiscore = true;
    }
}

void ball_move(Ball* b, Sprite* bs, double frame_time) {
    assert(b);
    assert(bs);
    assert(frame_time < 1.0);
    assert(frame_time >= 0.0);

    Sprite* ps   = &sprites.paddle.sprite;
    vec2s vel    = glms_vec2_scale(b->vel, BALL_SPEED * frame_time);
    vec2s newpos = glms_vec2_add(bs->pos, vel);

    if (is_oob(bs, newpos)) {
	sprites.paddle.lives -= 1;
	if (!sprites.paddle.lives) {
	    state = StateLost;
	    aud_sound_stop(playing);
	    aud_sound_play(AUD_LOST);
	    hiscore_check();
	} else {
	    aud_sound_play(AUD_DEATH);
	    ball_init(&sprites.ball, &sprites.ball.sprite, &sprites.paddle.sprite);
	}
    } else if (is_paddle_hit(bs, ps, newpos)) {
#ifndef NDEBUG
	fprintf(stderr, "Paddle hit\n");
#endif
        vec2s dist = get_aabb_dist(b, bs, ps);
	bounce(b, bs, ps, vel, dist, true);
    } else {
        unsigned brick_hits[VERT_COUNT];
        unsigned count = get_brick_hits(bs, newpos, brick_hits);
        if (count > 0) {
#ifndef NDEBUG
	    fprintf(stderr, "Brick hit: %u\n", count);
#endif
            vec2s dist = get_brick_closest(b, bs, count, brick_hits);
	    bounce(b, bs, ps, vel, dist, false);
	} else if (is_wall_hit(bs, newpos)) {
#ifndef NDEBUG
	    fprintf(stderr, "Wall hit\n");
#endif
	    vec2s dist = get_wall_dist(b, bs);
	    bounce(b, bs, ps, vel, dist, false);
        } else {
	    gfx_quad_set_pos(&bs->quad, newpos, bs->size);
        }
    }
}

bool is_won(void) {
    for (unsigned i = 0; i < BRICK_COLS * BRICK_ROWS; i++) {
	Brick* b = &sprites.bricks[i];
	assert(b);
        if (b->is_active && !b->is_solid && !b->is_destroyed) return false;
    }

    return true;
}

void game_update(double frame_time) {
    assert(frame_time <  1.0);
    assert(frame_time >= 0.0);

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
		if (state != StateRun) return;
	    }
	    // Account for rounding errors
	    ball_move(b, &b->sprite, frame_time - cr_time * (CR_COUNT - 1));
	    if (state != StateRun) return;

	    if (is_won()) {
		level++;
		if (level > LEVEL_COUNT) {
		    state = StateWon;
		    aud_sound_stop(playing);
		    aud_sound_play(AUD_WON);
		    hiscore_check();
		    level = 1;
		    return;
		} else {
		    aud_sound_stop(playing);
		    aud_sound_play(AUD_CLEAR);
		    level_reset();
		}
	    }
	}

	if (!playing || !ma_sound_is_playing(playing)) {
	    playing = aud_sound_play(AUD_MUSIC[level - 1]);
	}

	break;
    }
}

void level_render(void) {
    for (unsigned i = 0; i < BRICK_COLS * BRICK_ROWS; i++) {
	Brick* b = &sprites.bricks[i];
        if (b->is_active && !b->is_destroyed) {
            gfx_render_quad(&sprites.render, &b->sprite.quad);
        }
    }
}

void screen_render(Screen* s) {
    gfx_render_begin(&s->render);
    gfx_render_quad(&s->render, &s->quad);
    gfx_render_end(&s->render);
}

void text_render(const Text* t, ...) {
    gfx_font_begin(&fonts[t->size]);
    va_list ap;
    va_start(ap, t);
    int size = vsnprintf(NULL, 0, t->fmt, ap);
    char text[size + 1];
    vsnprintf(text, sizeof text, t->fmt, ap);
    va_end(ap);
    gfx_font_printf(&fonts[t->size], t->pos, t->col, text);
    gfx_font_end(&fonts[t->size]);
}

void screen_game(void) {
    screen_render(&bg);

    gfx_render_begin(&sprites.render);
    level_render();
    gfx_render_quad(&sprites.render, &sprites.ball.sprite.quad);
    gfx_render_quad(&sprites.render, &sprites.paddle.sprite.quad);
    if (sprites.paddle.lives > 1) {
	for (size_t i = 0; i < sprites.paddle.lives - 1; i++) {
	    gfx_render_quad(&sprites.render, &sprites.paddle.lives_sprites[i].quad);
	}
    }
    gfx_render_end(&sprites.render);

    text_render(&TEXT_SCORE,   sprites.paddle.score);
    text_render(&TEXT_HISCORE, hiscore);
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
	if (is_hiscore) text_render(&TEXT_NEWHISCORE);
	text_render(&TEXT_CONTINUE);
	break;
    case StateLost:
	screen_game();
	text_render(&TEXT_LOST);
	if (is_hiscore) text_render(&TEXT_NEWHISCORE);
	text_render(&TEXT_CONTINUE);
	break;
    }
}
