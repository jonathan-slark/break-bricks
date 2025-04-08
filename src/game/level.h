#pragma once

#include <cglm/struct.h> // vec2s

#include "../gfx/rend.h"
#include "../gfx/sprite.h"

// Function prototypes
void    level_load(void);
void    level_rend(Rend* r);
bool    level_isClear(void);
bool    level_next(void);
int     level_get(void);
int     level_getBrickCount(void);
Sprite* level_getBrickSprite(int brick);
void    level_destroyBrick(int brick);
