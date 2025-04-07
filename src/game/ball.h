#pragma once

#include "../gfx/rend.h"

// Function prototypes
void ball_init(void);
void ball_rend(Rend* r);
void ball_onPaddleMove(void);
void ball_release(void);
void ball_move(double frameTime);
