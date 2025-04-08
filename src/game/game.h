#pragma once

// Types
typedef enum {
    StateLoading,
    StateMenu,
    StateRun,
    StatePause,
    StateWon,
    StateLost
} State;

// Function prototypes
void  game_loaded(void);
void  game_pause(void);
void  game_quit(void);
void  game_click(void);
void  game_update(double frameTime);
void  game_render(void);
State game_getState(void);
void  game_lost(void);
