#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h> // GLFW*

#include "../main.h"
#include "../util.h"
#ifndef NDEBUG
#include "../gfx/gfx.h"
#endif
#include "ball.h"
#include "game.h"
#include "input.h"
#ifndef NDEBUG
#include "level.h"
#endif
#include "paddle.h"
#include "parallax.h"
#include "wall.h"

// Types

typedef struct {
    int key;
    void (*func)(void);
} Key;

typedef struct {
    int button;
    void (*func)(void);
} Button;

// Constants
static const Key KEYS[] = {
#ifndef NDEBUG
    { GLFW_KEY_N, (void (*)(void)) level_next },
    { GLFW_KEY_S, gfx_screenshot },
#endif
    { GLFW_KEY_SPACE,  game_togglePause },
    { GLFW_KEY_ESCAPE, game_quit }
};
static const Button BUTTONS[] = {
    { GLFW_MOUSE_BUTTON_LEFT, game_click }
};

// Function definitions

void input_keyDown(int key)
{
    for (size_t i = 0; i < COUNT(KEYS); i++) {
        if (KEYS[i].key == key) {
            (*KEYS[i].func)();
        }
    }
}

void input_buttonDown(int button)
{
    for (size_t i = 0; i < COUNT(BUTTONS); i++) {
        if (BUTTONS[i].button == button) {
            (*BUTTONS[i].func)();
        }
    }
}

void input_update(void)
{
    switch (game_getState()) {
        case StateLoading:
	    [[fallthrough]];
        case StateMenu:
	    [[fallthrough]];
        case StatePause:
	    [[fallthrough]];
        case StateWon:
	    [[fallthrough]];
        case StateLost:
	    break;
        case StateRun:
	    vec2s pos = main_getMousePos();
	    pos.x     = CLAMP(pos.x, WALL_LEFT, SCR_WIDTH - paddle_getSprite().size.s - WALL_RIGHT);
	    paddle_setX(pos.x);
	    ball_onPaddleMove();
	    parallax_onPaddleMove();
	    // Don't allow cursor to move away from paddle
	    main_setMousePos(pos);
	    break;
    }
}

// Move mouse cursor back to where the paddle is
void input_onContinue(void)
{
    main_setMousePos(paddle_getSprite().pos);
}
