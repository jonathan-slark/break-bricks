#include "../gfx/screen.h"
#include "asset.h"
#include "ball.h"
#include "game.h"
#include "level.h"
#include "paddle.h"
#include "text.h"

// Function prototypes
static void drawGame(void);

// Constants
static const Text TEXT_PAUSED = { FontLarge,  {{ 880,  600 }}, {{ 1.0f, 1.0f, 1.0f }}, "Paused." };
static const Text TEXT_MENU   =
{
    FontMedium, {{ 680,  860 }}, {{ 0.6f, 0.6f, 0.6f }},
    "Use the mouse to control the paddle.\n"
    "Click mouse button to release the ball.\n"
    "Press space to pause.\n"
    "Press escape to quit.\n"
    "\n"
    "Click mouse button to continue."
};

// Function declarations

void drawGame(void)
{
    screen_rend(asset_getBg());

    Rend* r = asset_getSpriteRend();
    rend_begin(*r);
    rend_sprite(r, paddle_getSprite());
    rend_sprite(r, ball_getSprite());
    level_render(0, r);
    rend_end(r);
}

void draw_frame(void)
{
    switch (game_getState())
    {
        case StateLoading:
            screen_rend(asset_getLoading());
            break;
        case StateMenu:
            screen_rend(asset_getLoading());
	    text_rend(asset_getFont(TEXT_MENU.size), TEXT_MENU);
            break;
        case StatePause:
            drawGame();
	    text_rend(asset_getFont(TEXT_PAUSED.size), TEXT_PAUSED);
            break;
        case StateRun:
            drawGame();
            break;
        case StateWon:
            drawGame();
            //text_render(&TEXT_WON);
            //if (is_hiscore) text_render(&TEXT_NEWHISCORE);
            //text_render(&TEXT_CONTINUE);
            break;
        case StateLost:
            break;
    }
}
