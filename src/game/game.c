#include "../main.h"
#include "audio.h"
#include "ball.h"
#include "game.h"
#include "hiscore.h"
#include "level.h"

// Variables
static State state = StateLoading;

// Function definitions

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
	    audio_continueMusic();
	    break;
	case StateRun:
	    state = StatePause;
	    audio_stopMusic();
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
	    main_quit();
	    break;
	case StateRun:
	    audio_stopMusic();
	    [[fallthrough]];
	case StatePause:
	case StateWon:
	case StateLost:
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
	    state = StateMenu;
	    break;
	case StateMenu:
	    state = StateRun;
	    audio_playMusic(level_get());
	    break;
	case StateRun:
	    ball_release();
	    break;
    }
}

void game_clear(void)
{
    audio_stopMusic();
    audio_playSound(SoundClear);
    audio_playMusic(level_get());
}

void game_won(void)
{
    state = StateWon;
    audio_stopMusic();
    audio_playSound(SoundWon);
    hiscore_check();
}

void game_update(double frameTime)
{
    switch (state)
    {
	case StateLoading:
	case StateMenu:
	case StatePause:
	case StateWon:
	case StateLost:
	    break;
	case StateRun:
	    ball_move(frameTime);

	    if (level_isClear())
	    {
		if (level_next())
		{
		    game_clear();
		}
		else
		{
		    game_won();
		}
	    }

	    break;
    }
}

void game_setState(State newState)
{
    state = newState;
}

State game_getState(void)
{
    return state;
}
