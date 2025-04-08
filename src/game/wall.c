#include <cglm/struct.h> // vec2s

#include "../main.h"
#include "../gfx/sprite.h"
#include "wall.h"

// Constants
const float WALL_TOP   = 72.0f;
const float WALL_LEFT  = 192.0f;
const float WALL_RIGHT = 192.0f;

Sprite walls[WallCount];

// Create dummy sprites for collisions
void wall_init(void)
{
    vec2s unused = {{ 0, 0 }}; // texOffset and texSize are unused

    vec2s pos  = {{ 0, 0 }};
    vec2s size = {{ SCR_WIDTH, WALL_TOP }};
    walls[WallTop] = sprite_create(pos, size, unused, unused);

    size = (vec2s) {{ WALL_LEFT, SCR_HEIGHT }};
    walls[WallLeft] = sprite_create(pos, size, unused, unused);

    pos  = (vec2s) {{ SCR_WIDTH - WALL_RIGHT, 0 }};
    size = (vec2s) {{ WALL_RIGHT, SCR_HEIGHT }};
    walls[WallRight] = sprite_create(pos, size, unused, unused);
}

Sprite wall_getSprite(Wall w)
{
    return walls[w];
}
