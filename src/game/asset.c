#include <stdlib.h>     // atexit, srand
#include <time.h>       // timespec*

#include "../main.h"
#include "../gfx/gfx.h"
#include "../gfx/screen.h"
#include "asset.h"
#include "game.h"
#include "paddle.h"

// Function prototypes
static void loadingLoad(void);
static void loadingUnload(void);
static void bgLoad(void);
static void bgUnload(void);

// Constants
static const char FILE_LOADING[] = "gfx/loading.png";
static const char FILE_BG[]      = "gfx/background.png";

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
void asset_loading(void)
{
    gfx_init();
    atexit(gfx_term);

    loadingLoad();
    atexit(loadingUnload);
}

void asset_load(void)
{
    // Decent random seed: https://stackoverflow.com/q/58150771
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    srand(ts.tv_nsec);

    bgLoad();
    atexit(bgUnload);

    paddle_init();
}

void asset_loaded(void)
{
    game_setState(StateMenu);
}

Screen asset_getLoading(void)
{
    return loading;
}

Screen asset_getBg(void)
{
    return bg;
}
