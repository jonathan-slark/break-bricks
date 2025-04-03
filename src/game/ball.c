#include <cglm/struct.h> // vec2s

#include "../gfx/sprite.h"
#include "../main.h"
#include "ball.h"
#include "paddle.h"

// Function prototypes
static vec2s getStuckPos(void);

// Constants
static const vec2s SIZE       = {{ 24, 24 }};
static const vec2s TEX_OFFSET = {{ 528, 0 }};

// Variables
static bool   isStuck;
//static vec2s  vel;
static Sprite sprite;

// Function declarations

vec2s getStuckPos(void)
{
    Sprite ps = paddle_getSprite();
    return (vec2s) {{ ps.pos.x + ps.size.s / 2.0f - SIZE.s / 2.0f, ps.pos.y - SIZE.t }};
}

void ball_init(void)
{
    isStuck = true;
    sprite  = sprite_create(getStuckPos(), SIZE, TEX_OFFSET, (vec2s) {{ SCR_WIDTH, SCR_HEIGHT }} );
}

Sprite ball_getSprite(void)
{
    return sprite;
}

void ball_onPaddleMove(void)
{
    if (isStuck)
    {
	sprite_setPos(&sprite, getStuckPos());
    }
}
