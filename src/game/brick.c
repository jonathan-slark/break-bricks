#include <ctype.h> // isdigit

#include "../main.h"
#include "config.h"
#include "brick.h"

// Constants
static const vec2s    BRICK_SIZE  = {{ 128, 32 }};
static const vec2s    BRICK_SINGLE_OFFSETS[] = {
    {{ 0,   64 }}, // blue,   id = 0
    {{ 128, 64 }}, // green,  id = 1
    {{ 256, 64 }}, // orange, id = 2
    {{ 0,   96 }}, // purple, id = 3
    {{ 128, 96 }}, // red,    id = 4
    {{ 256, 96 }}  // yellow, id = 5
};
static const vec2s    BRICK_SOLID_OFFSETS[] = {
    {{ 0,   0 }},  // blue,   id = a
    {{ 128, 0 }},  // green,  id = b
    {{ 256, 0 }},  // orange, id = c
    {{ 0,   32 }}, // purple, id = d
    {{ 128, 32 }}, // red,    id = e
    {{ 256, 32 }}, // yellow, id = f
};

// Function declarations

Brick brick_create(char id, int col, int row)
{
    Brick b;

    bool isSolid = !isdigit(id);

    b.isActive    = true;
    b.isSolid     = isSolid;
    b.isDestroyed = false;

    vec2s pos    = {{ BG_WALL_LEFT + col * BRICK_SIZE.s, BG_WALL_TOP + row * BRICK_SIZE.t }};
    vec2s offset = isSolid ? BRICK_SOLID_OFFSETS[id - 'a'] : BRICK_SINGLE_OFFSETS[id - '0'];
    b.sprite     = sprite_create(pos, BRICK_SIZE, offset, (vec2s) {{ SCR_WIDTH, SCR_HEIGHT }});

    return b;
}
