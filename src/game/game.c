#include "../main.h"
#include "audio.h"
#include "ball.h"
#include "game.h"
#include "hiscore.h"
#include "level.h"

// Variables
static State state = StateLoading;

// Function definitions

void game_loaded(void)
{
    state = StateMenu;
}

void game_pause(void)
{
    switch (state) {
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
	    audio_pauseMusic();
	    break;
    };
}

void game_quit(void)
{
    switch (state) {
	case StateLoading:
	    break;
	case StateMenu:
	    main_quit();
	    break;
	case StateRun:
	case StatePause:
	    audio_stopMusic();
	    [[fallthrough]];
	case StateWon:
	case StateLost:
	    state = StateMenu;
	    break;
    }
}

void game_click(void)
{
    switch (state) {
	case StateLoading:
	case StatePause:
	    break;
	case StateWon:
	case StateLost:
	    state = StateMenu;
	    hiscore_resetIsHi();
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

void game_lost(void)
{
    state = StateLost;
    audio_stopMusic();
    audio_playSound(SoundLost);
}

void game_update(double frameTime)
{
    switch (state) {
	case StateLoading:
	case StateMenu:
	case StatePause:
	case StateWon:
	case StateLost:
	    break;
	case StateRun:
	    ball_move(frameTime);

	    if (level_isClear()) {
		ball_init();
		audio_stopMusic();

		if (level_next()) {
		    // Level clear
		    audio_playSound(SoundClear);
		    audio_playMusic(level_get());
		} else {
		    // Game won!
		    state = StateWon;
		    audio_playSound(SoundWon);
		    hiscore_check();
		}
	    }

	    break;
    }
}

State game_getState(void)
{
    return state;
}
