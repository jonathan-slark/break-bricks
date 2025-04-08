#include "../gfx/screen.h"
#include "asset.h"
#include "ball.h"
#include "game.h"
#include "hiscore.h"
#include "level.h"
#include "paddle.h"
#include "text.h"

// Function prototypes
static void drawGame(void);

// Constants
static const Text TEXT_PAUSED     = { FontLarge,  {{ 880,  600 }}, {{ 1.0f, 1.0f, 1.0f }}, "Paused." };
static const Text TEXT_SCORE      = { FontLarge,  {{ 192,  56  }}, {{ 1.0f, 1.0f, 1.0f }}, "%i" };
static const Text TEXT_HISCORE    = { FontLarge,  {{ 1573, 56  }}, {{ 1.0f, 1.0f, 1.0f }}, "%i" };
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
    screen_rend(asset_getBg());

    Rend* r = asset_getSpriteRend();
    rend_begin(*r);
    level_rend(r);
    paddle_rend(r);
    ball_rend(r);
    rend_end(r);

    text_rend(TEXT_SCORE, paddle_getScore());
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
            break;
        case StatePause:
            drawGame();
	    text_rend(TEXT_PAUSED);
            break;
        case StateRun:
            drawGame();
            break;
        case StateWon:
            drawGame();
            text_rend(TEXT_WON);
            if (hiscore_isHi()) text_rend(TEXT_NEWHISCORE);
            text_rend(TEXT_CONTINUE);
            break;
        case StateLost:
	    drawGame();
            text_rend(TEXT_LOST);
            if (hiscore_isHi()) text_rend(TEXT_NEWHISCORE);
            text_rend(TEXT_CONTINUE);
            break;
    }
}
