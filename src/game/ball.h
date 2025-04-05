#pragma once

#include "../gfx/sprite.h"

// Function prototypes
void   ball_init(void);
Sprite ball_getSprite(void);
void   ball_onPaddleMove(void);
void   ball_release(void);
void   ball_move(double frameTime);
