#include "../main.h"
#include "audio.h"
#include "ball.h"
#include "game.h"
#include "hiscore.h"
#include "input.h"
#include "level.h"
#include "paddle.h"

// Function prototypes
static void resetGame(void);
static void startGame(void);
static void gameWon(void);
static void levelClear(void);

// Variables
static State state = StateLoading;

// Function definitions

void game_loaded(void)
{
    state = StateMenu;
}

void game_pause(void)
{
    if (state == StateRun) {
	state = StatePause;
	audio_pauseMusic();
    }
}

void game_continue(void)
{
    if (state == StatePause) {
	state = StateRun;
	audio_continueMusic();
	input_onContinue();
    }
}

void game_togglePause(void)
{
    switch (state) {
	case StateLoading:
	case StateMenu:
	case StateWon:
	case StateLost:
	    break;
	case StatePause:
	    game_continue();
	    break;
	case StateRun:
	    game_pause();
	    break;
    };
}

void resetGame(void)
{
    state = StateMenu;
    paddle_resetStats();
    hiscore_resetIsHi();
    level_reset();
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
	    resetGame();
	    break;
    }
}

void startGame(void)
{
    state = StateRun;
    audio_playMusic(level_getCurrent());
    ball_init();
}

void game_click(void)
{
    switch (state) {
	case StateLoading:
	case StatePause:
	    break;
	case StateWon:
	case StateLost:
	    resetGame();
	    break;
	case StateMenu:
	    startGame();
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
    hiscore_check();
}

void gameWon(void)
{
    state = StateWon;
    audio_stopMusic();
    audio_playSound(SoundWon);
    hiscore_check();
}

void levelClear(void)
{
    audio_stopMusic();
    audio_playSound(SoundClear);
    audio_playMusic(level_getCurrent());
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
		if (level_next()) levelClear(); else gameWon();
	    }

	    break;
    }
}

State game_getState(void)
{
    return state;
}
