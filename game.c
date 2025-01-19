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
    union {
        vec2s pos;
        Quad quad;
    };
    vec2s size;
} Sprite;

typedef struct {
    Quad     quad;
    Renderer render;
} BG;


typedef struct {
    Sprite   paddle;
    Renderer render;
} Sprites;

// Function prototypes

void paddle_init(void);
static void quit(void);

// Requires above types and functions
#include "config.h"

// Constants
static const size_t sprite_count = 200;

// Variables
BG bg;
Sprites sprites;

// Function implementations

void paddle_init(void) {
    vec2s mousepos = main_getmousepos();
    vec2s pos = {{ mousepos.x, SCR_HEIGHT - PADDLE_SIZE.t }};
    sprites.paddle.quad = gfx_quad_create(&sprites.render, pos, PADDLE_SIZE,
	    PADDLE_OFFSET);
    sprites.paddle.size = PADDLE_SIZE;
}

void game_load(void) {
    gfx_init();

    vec2s pos  = {{ 0, 0 }};
    vec2s size = {{ SCR_WIDTH, SCR_HEIGHT }};
    bg.render = gfx_render_create(1, BACKGROUND);
    bg.quad   = gfx_quad_create(&bg.render, pos, size, pos);

    sprites.render = gfx_render_create(sprite_count, SPRITE_SHEET);
    paddle_init();

    state = StateRun;
}

void game_unload(void) {
    gfx_render_delete(&bg.render);
    gfx_term();
}

void quit(void) {
    main_quit();
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

void game_buttondown([[maybe_unused]] int button) {
}

void game_buttonup([[maybe_unused]] int button) {
}

void game_input([[maybe_unused]] double frametime) {
    switch(state) {
    case StateRun:
	vec2s mousepos = main_getmousepos();
	mousepos.x = CLAMP(
		mousepos.x,
		WALL_WIDTH,
		SCR_WIDTH - sprites.paddle.size.x - WALL_WIDTH
	);
	vec2s newpos = (vec2s) {{ mousepos.x, sprites.paddle.pos.y }};
	gfx_quad_setpos(&sprites.paddle.quad, newpos, sprites.paddle.size);
	// Don't allow cursor to move away from paddle
	main_setmousepos(mousepos);
	break;
    default:
	// VOID
    }
}

void game_update([[maybe_unused]] double frametime) {
}

void game_render(void) {
    gfx_render_begin(&bg.render);
    gfx_render_quad(&bg.render, &bg.quad);
    gfx_render_end(&bg.render);

    gfx_render_begin(&sprites.render);
    gfx_render_quad(&sprites.render, &sprites.paddle.quad);
    gfx_render_end(&sprites.render);
}
