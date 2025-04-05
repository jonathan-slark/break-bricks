#include <stdlib.h>     // atexit, srand
#include <time.h>       // timespec*

#include "../main.h"
#include "../gfx/font.h"
#include "../gfx/gfx.h"
#include "../gfx/rend.h"
#include "../gfx/screen.h"
#include "audio.h"
#include "asset.h"
#include "ball.h"
#include "game.h"
#include "hiscore.h"
#include "level.h"
#include "paddle.h"

// Function prototypes
static void loadLoading(void);
static void unloadLoading(void);
static void loadBg(void);
static void unloadBg(void);
static void loadSpriteRend(void);
static void unloadSpriteRend(void);
static void loadFonts(void);
static void unloadFonts(void);

// Constants
static const char   FILE_LOADING[] = "gfx/loading.png";
static const char   FILE_BG[]      = "gfx/background.png";
static const char   SPRITE_SHEET[] = "gfx/spritesheet.png";
static const size_t SPRITE_COUNT   = 200;
static const char   FONT_FILE[]    = "font/JupiteroidRegular.ttf";
static const float  FONT_HEIGHTS[] = { 64.0f, 40.0f };

// Variables
static Screen loading;
static Screen bg;
static Rend   spriteRend;
static Font   fonts[FontSizeCount];

// Function declarations

void loadLoading(void)
{
    loading = screen_load(FILE_LOADING);
}

void unloadLoading(void)
{
    screen_unload(loading);
}

void loadBg(void)
{
    bg = screen_load(FILE_BG);
}

void unloadBg(void)
{
    screen_unload(bg);
}

void loadSpriteRend(void)
{
    spriteRend = rend_load(SPRITE_COUNT, SPRITE_SHEET);
}

void unloadSpriteRend(void)
{
    rend_unload(spriteRend);
}

void loadFonts(void)
{
    for (size_t i = 0; i < FontSizeCount; i++)
    {
        fonts[i] = font_load(FONT_HEIGHTS[i], FONT_FILE);
    }
}

void unloadFonts(void)
{
    for (size_t i = 0; i < FontSizeCount; i++)
    {
        font_unload(fonts[i]);
    }
}

// Do the minimum required to get a loading screen
void asset_loading(void)
{
    gfx_init();
    atexit(gfx_term);

    loadLoading();
    atexit(unloadLoading);
}

void asset_load(void)
{
    // Decent random seed: https://stackoverflow.com/q/58150771
    struct timespec ts;
    timespec_get(&ts, TIME_UTC);
    srand(ts.tv_nsec);

    loadBg();
    atexit(unloadBg);

    loadSpriteRend();
    atexit(unloadSpriteRend);

    loadFonts();
    atexit(unloadFonts);

    hiscore_load();
    atexit(hiscore_save);

    audio_load();
    atexit(audio_unload);

    paddle_init();
    ball_init();
    level_load();
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

Rend* asset_getSpriteRend(void)
{
    return &spriteRend;
}

Font* asset_getFont(FontSize size)
{
    return &fonts[size];
}
