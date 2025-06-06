#include <stdlib.h> // atexit

#include "../main.h"
#include "../util.h"
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
#include "parallax.h"
#include "wall.h"

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
static const char* FILE_BGS[COUNT] = {
    "gfx/background1.png",
    "gfx/background2.png",
    "gfx/background3.png",
    "gfx/background4.png",
    "gfx/background5.png",
    "gfx/background6.png",
    "gfx/background7.png"
};
static const char   FILE_LOADING[] = "gfx/loading.png";
static const char   SPRITE_SHEET[] = "gfx/spritesheet.png";
static const size_t SPRITE_COUNT   = 200;
static const char   FONT_FILE[]    = "font/JupiteroidRegular.ttf";
static const float  FONT_HEIGHTS[] = { 64.0f, 40.0f };

// Variables
static Screen loading;
static Screen bgs[COUNT];
static Rend   spriteRend;
static Font   fonts[FontSizeCount];

// Function definitions

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
    for (int i = 0; i < COUNT; i++) {
	bgs[i] = screen_load(FILE_BGS[i]);
    }
}

void unloadBg(void)
{
    for (int i = 0; i < COUNT; i++) {
	screen_unload(bgs[i]);
    }
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
    for (size_t i = 0; i < FontSizeCount; i++) {
        fonts[i] = font_load(FONT_HEIGHTS[i], FONT_FILE);
    }
}

void unloadFonts(void)
{
    for (size_t i = 0; i < FontSizeCount; i++) {
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
    util_randomSeed();

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
    ball_init();     // Requires paddle_init
    level_load();
    wall_init();

    parallax_load(); // Requires paddle_init
    atexit(parallax_unload);
}

Screen asset_getLoading(void)
{
    return loading;
}

Screen asset_getBg(int level)
{
    return bgs[level];
}

Rend* asset_getSpriteRend(void)
{
    return &spriteRend;
}

Font* asset_getFont(FontSize size)
{
    return &fonts[size];
}
