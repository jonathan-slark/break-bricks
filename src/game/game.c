#include "game.h"

// Variables
State state = StateLoading;

// Function declarations

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
