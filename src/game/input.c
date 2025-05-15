#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h> // GLFW*

#include "../main.h"
#include "../util.h"
#ifndef NDEBUG
#include "../gfx/gfx.h"
#endif
#include "audio.h"
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

// Function declarations
static void nextLevel(void);

// Constants
static const Key KEYS[] = {
#ifndef NDEBUG
    { GLFW_KEY_N, (void (*)(void)) nextLevel },
    { GLFW_KEY_S, gfx_screenshot },
#endif
    { GLFW_KEY_SPACE,  game_togglePause },
    { GLFW_KEY_ESCAPE, game_quit }
};
static const Button BUTTONS[] = {
    { GLFW_MOUSE_BUTTON_LEFT, game_click }
};

// Function definitions

#ifndef NDEBUG
void nextLevel(void) 
{
    level_next();
    audio_stopMusic();
    audio_playMusic(level_getCurrent());
}
#endif

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
	    double mouseDx = main_getMouseDx();
	    if (mouseDx != 0.0) {
		float newX = paddle_getSprite().pos.x + (float) mouseDx;
		newX = CLAMP(newX, WALL_LEFT, SCR_WIDTH - paddle_getSprite().size.s - WALL_RIGHT);
		paddle_setX(newX);

		ball_onPaddleMove();
		parallax_onPaddleMove();
	    }
	    break;
    }
}

// Move mouse cursor back to where the paddle is
void input_onContinue(void)
{
    main_setMousePos(paddle_getSprite().pos);
}
