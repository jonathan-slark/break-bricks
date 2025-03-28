#include "../gfx/screen.h"
#include "asset.h"
#include "game.h"

void draw_frame(void)
{
    switch (game_getState())
    {
        case StateLoading:
            screen_rend(asset_getLoading());
            break;
        case StateMenu:
            screen_rend(asset_getLoading());
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
