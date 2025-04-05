#include <cglm/struct.h> // vec2s

#include "../main.h"
#include "../gfx/sprite.h"
#include "wall.h"

// Constants
const float WALL_TOP   = 72.0f;
const float WALL_LEFT  = 192.0f;
const float WALL_RIGHT = 192.0f;

bool wall_isCollisionX(Sprite s)
{
    return s.pos.x <= WALL_LEFT || s.pos.x + s.size.s >= SCR_WIDTH - WALL_RIGHT;
}

bool wall_isCollisionY(Sprite s)
{
    return s.pos.y <= WALL_TOP;
}
