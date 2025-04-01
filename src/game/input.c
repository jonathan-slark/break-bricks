#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h> // GLFW*

#include "../main.h"
#include "../util.h"
#include "input.h"
#include "game.h"

// Function prototypes
static void pause(void);
static void quit(void);

// Types
typedef struct
{
    int key;
    void (*func)(void);
} Key;

// Constants
static const Key KEYS[] =
{
    { GLFW_KEY_SPACE,  pause },
    { GLFW_KEY_ESCAPE, quit }
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

void input_keyUp([[maybe_unused]] int key)
{
    // VOID
}


void pause(void)
{
    switch (game_getState())
    {
	case StateLoading:
	case StateMenu:
	case StateWon:
	case StateLost:
	    break;
	case StatePause:
	    game_setState(StateRun);
	    break;
	case StateRun:
	    game_setState(StatePause);
	    break;
    };
}

void quit(void)
{
    switch (game_getState())
    {
	case StateLoading:
	    break;
	case StateMenu:
	    //hiscore_save();
	    main_quit();
	    break;
	case StateRun:
	case StatePause:
	    //aud_sound_stop(playing);
	    [[fallthrough]];
	case StateWon:
	case StateLost:
	    //level_fullreset();
	    game_setState(StateMenu);
	    break;
    }
}

void input_update(void)
{
}
