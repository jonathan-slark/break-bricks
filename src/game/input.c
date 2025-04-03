#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h> // GLFW*

#include "../main.h"
#include "../util.h"
#include "ball.h"
#include "config.h"
#include "game.h"
#include "input.h"
#include "paddle.h"

// Types

typedef struct
{
    int key;
    void (*func)(void);
} Key;

typedef struct
{
    int button;
    void (*func)(void);
} Button;

// Constants

static const Key KEYS[] =
{
    { GLFW_KEY_SPACE,  game_pause },
    { GLFW_KEY_ESCAPE, game_quit }
};

static const Button BUTTONS[] =
{
    { GLFW_MOUSE_BUTTON_LEFT, game_click }
};

// Function declarations

void input_keyDown(int key)
{
    for (size_t i = 0; i < COUNT(KEYS); i++)
    {
        if (KEYS[i].key == key)
        {
            (*KEYS[i].func)();
        }
    }
}

void input_buttonDown(int button)
{
    for (size_t i = 0; i < COUNT(BUTTONS); i++)
    {
        if (BUTTONS[i].button == button)
        {
            (*BUTTONS[i].func)();
        }
    }
}

void input_update(void)
{
    switch (game_getState())
    {
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
	    pos.x     = CLAMP(pos.x, BG_WALL_LEFT, SCR_WIDTH - paddle_getSprite().size.s - BG_WALL_RIGHT);
	    paddle_setX(pos.x);
	    ball_onPaddleMove();
	    // Don't allow cursor to move away from paddle
	    main_setMousePos(pos);
	    break;
    }
}
