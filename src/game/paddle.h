#pragma once

#include "../gfx/rend.h"
#include "../gfx/sprite.h"

// Function prototypes
void   paddle_resetStats(void);
void   paddle_init(void);
void   paddle_setX(float x);
Sprite paddle_getSprite(void);
void   paddle_rend(Rend* r);
int    paddle_getScore(void);
void   paddle_incScore(int s);
bool   paddle_lifeLost(void);
