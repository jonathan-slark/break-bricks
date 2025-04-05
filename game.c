#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h> // GLFW*
#include <stdlib.h>     // atexit, srand
#include <time.h>       // timespec*

#include "gfx.h"
#include "main.h"
#include "screen.h"
#include "util.h"

// Types

enum
{
    StateLoading,
    StateMenu,
    StateRun,
    StatePause,
    StateWon,
    StateLost
} state = StateLoading;

typedef struct
{
    int key;
    void (*func)(void);
} Key;

// Function prototypes
static void loadingLoad(void);
static void loadingUnload(void);
static void bgLoad(void);
static void bgUnload(void);
static void quit(void);

// Constants
static const char FILE_LOADING[] = "gfx/loading.png";
static const char FILE_BG[]      = "gfx/background.png";
static const Key KEYS[] =
{
    { GLFW_KEY_ESCAPE, quit }
};

// Variables
Screen loading, bg;

// Function declarations

void loadingLoad(void)
{
    loading = screen_load(FILE_LOADING);
}

void loadingUnload(void)
{
    screen_unload(loading);
}

void bgLoad(void)
{
    bg = screen_load(FILE_BG);
}

void bgUnload(void)
{
    screen_unload(bg);
}

// Do the minimum required to get a loading screen
void game_loading(void)
{
    gfx_init();
    atexit(gfx_term);

    loadingLoad();
    atexit(loadingUnload);
}

void game_load(void)
{
    // Decent random seed: https://stackoverflow.com/q/58150771
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    srand(ts.tv_nsec);

    bgLoad();
    atexit(bgUnload);
}

void game_loaded(void)
{
    state = StateMenu;
}

void quit(void)
{
    main_quit();
}

void game_keyDown(int key)
{
    for (size_t i = 0; i < COUNT(KEYS); i++)
    {
        if (KEYS[i].key == key)
	{
            (*KEYS[i].func)();
        }
    }
}

void game_keyUp([[maybe_unused]] int key)
{
    // VOID
}

void game_input(void)
{
}

void game_update([[maybe_unused]] double frameTime)
{
}

void game_render(void)
{
    switch (state)
    {
	case StateLoading:
	    screen_rend(loading);
	    break;
	case StateMenu:
	    screen_rend(loading);
	    //text_render(&TEXT_MENU);
	    break;
	case StatePause:
	    //screen_game();
	    //text_render(&TEXT_PAUSED);
	    break;
	case StateRun:
	    //screen_game();
	    break;
	case StateWon:
	    //screen_game();
	    //text_render(&TEXT_WON);
	    //if (is_hiscore) text_render(&TEXT_NEWHISCORE);
	    //text_render(&TEXT_CONTINUE);
	    break;
	case StateLost:
	    break;
    }
}
