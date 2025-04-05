#include "../main.h"
#include "audio.h"
#include "ball.h"
#include "game.h"
#include "hiscore.h"

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
            hiscore_save();
            main_quit();
            break;
        case StateRun:
        case StatePause:
	    audio_stopMusic();
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
	ball_release();
        break;
    }
}

void game_update(double frameTime)
{
    ball_move(frameTime);
}

void game_setState(State newState)
{
    state = newState;
}

State game_getState(void)
{
    return state;
}
