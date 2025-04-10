#include <cglm/struct.h> // vec3s

#include "../gfx/gfx.h"
#include "../gfx/screen.h"
#include "asset.h"
#include "ball.h"
#include "game.h"
#include "hiscore.h"
#include "level.h"
#include "paddle.h"
#include "parallax.h"
#include "text.h"

// Function prototypes
static void drawGame(void);

// Constants
static const vec3s BLACK = {{ 0.0f, 0.0f, 0.0f }};
static const Text TEXT_PAUSED     = { FontLarge,  {{ 880,  600 }}, {{ 1.0f, 1.0f, 1.0f }}, "Paused." };
static const Text TEXT_SCORE      = { FontLarge,  {{ 180,  50  }}, {{ 1.0f, 1.0f, 1.0f }}, "Score: %i" };
static const Text TEXT_HISCORE    = { FontLarge,  {{ 1440, 50  }}, {{ 1.0f, 1.0f, 1.0f }}, "Hiscore: %i" };
static const Text TEXT_LEVEL      = { FontLarge,  {{ 840, 50  }}, {{ 1.0f, 1.0f, 1.0f }}, "Level: %i of %i" };
static const Text TEXT_LOST       = { FontLarge,  {{ 840, 600 }}, {{ 1.0f, 1.0f, 1.0f }}, "Game over." };
static const Text TEXT_NEWHISCORE = { FontLarge,  {{ 820, 664 }}, {{ 1.0f, 1.0f, 1.0f }}, "New hiscore!" };
static const Text TEXT_WON        = { FontLarge,  {{ 855, 600 }}, {{ 1.0f, 1.0f, 1.0f }}, "You won!" };
static const Text TEXT_CONTINUE   = { FontMedium, {{ 745, 860 }}, {{ 1.0f, 1.0f, 1.0f }},
    "Click mouse button to continue." };
static const Text TEXT_MENU =
{
    FontMedium, {{ 680,  860 }}, {{ 0.6f, 0.6f, 0.6f }},
    "Use the mouse to control the paddle.\n"
    "Click mouse button to release the ball.\n"
    "Press space to pause.\n"
    "Press escape to quit.\n"
    "\n"
    "Click mouse button to continue."
};

// Function definitions

void drawGame(void)
{
    gfx_clear(BLACK);
    parallax_rend();
    screen_rend(asset_getBg());

    Rend* r = asset_getSpriteRend();
    rend_begin(*r);
    level_rend(r);
    paddle_rend(r);
    ball_rend(r);
    rend_end(r);

    text_rend(TEXT_SCORE, paddle_getScore());
    text_rend(TEXT_LEVEL, level_getCurrent() + 1, level_getCount());
    text_rend(TEXT_HISCORE, hiscore_getHi());
}

void draw_frame(void)
{
    switch (game_getState()) {
        case StateLoading:
            screen_rend(asset_getLoading());
            break;
        case StateMenu:
            screen_rend(asset_getLoading());
	    text_rend(TEXT_MENU);
	    text_flush();
            break;
        case StatePause:
            drawGame();
	    text_rend(TEXT_PAUSED);
	    text_flush();
            break;
        case StateRun:
            drawGame();
	    text_flush();
            break;
        case StateWon:
            drawGame();
            text_rend(TEXT_WON);
            if (hiscore_isHi()) text_rend(TEXT_NEWHISCORE);
            text_rend(TEXT_CONTINUE);
	    text_flush();
            break;
        case StateLost:
	    drawGame();
            text_rend(TEXT_LOST);
            if (hiscore_isHi()) text_rend(TEXT_NEWHISCORE);
            text_rend(TEXT_CONTINUE);
	    text_flush();
            break;
    }
}
