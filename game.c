#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h> // GLFW*
#include <stdlib.h>     // atexit

#include "gfx.h"
#include "main.h"
#include "screen.h"
#include "util.h"

// Types
typedef struct
{
    int key;
    void (*func)(void);
} Key;

// Function prototypes
static void bgLoad(void);
static void bgUnload(void);
static void quit(void);

// Constants
static const char BG_FILE[] = "gfx/background.png";
static const Key KEYS[] =
{
    { GLFW_KEY_ESCAPE, quit }
};

// Variables
Screen bg;

// Function declarations

void bgLoad(void)
{
    bg = screen_load(BG_FILE);
}

void bgUnload(void)
{
    screen_unload(bg);
}

void game_load(void)
{
    gfx_init();
    atexit(gfx_term);

    bgLoad();
    atexit(bgUnload);
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

void game_render(void)
{
    screen_rend(bg);
}
