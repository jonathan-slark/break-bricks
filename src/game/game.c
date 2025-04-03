#include "../main.h"
#include "game.h"

// Variables
static State state = StateLoading;

// Function declarations

void game_pause(void)
{
    switch (state)
    {
        case StateLoading:
        case StateMenu:
        case StateWon:
        case StateLost:
            break;
        case StatePause:
            state = StateRun;
            break;
        case StateRun:
            state = StatePause;
            break;
    };
}

void game_quit(void)
{
    switch (state)
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
            state = StateMenu;
            break;
    }
}

void game_click(void)
{
    switch (state)
    {
    case StateLoading:
        break;
    case StatePause:
        state = StateRun;
        break;
    case StateWon:
    case StateLost:
        //level_fullreset();
        state = StateMenu;
        break;
    case StateMenu:
        state = StateRun;
        break;
    case StateRun:
#if 0
        Ball *b = &sprites.ball;

        if (b->is_stuck)
        {
            b->vel = BALL_RELEASE[random(0, COUNT(BALL_RELEASE) - 1)];
            b->vel = glms_vec2_normalize(b->vel);

            b->is_stuck = false;
        }
#endif
        break;
    }
}

void game_update([[maybe_unused]] double frameTime)
{
}

void game_setState(State newState)
{
    state = newState;
}

State game_getState(void)
{
    return state;
}
