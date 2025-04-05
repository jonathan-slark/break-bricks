#include <assert.h>      // assert
#include <cglm/struct.h> // vec2s

#include "../main.h"
#include "../util.h"
#include "../gfx/sprite.h"
#include "ball.h"
#include "paddle.h"

// Function prototypes
static vec2s getStuckPos(void);

// Constants
static const vec2s SIZE          = {{ 24, 24 }};
static const vec2s TEX_OFFSET    = {{ 528, 0 }};
// Choose a random release vector for the ball
static const vec2s RELEASE_VEC[] = { {{ -0.5f, -0.5f }}, {{  0.5f, -0.5f }} };
static const unsigned SPEED      = 750;  // Pixels per second

// Variables
static bool   isStuck;
static vec2s  vel;
static Sprite sprite;

// Function definitions

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

void ball_release(void)
{
    if (isStuck)
    {
	vel = RELEASE_VEC[util_randomInt(0, COUNT(RELEASE_VEC) - 1)];
	vel = glms_vec2_normalize(vel);
	isStuck = false;
    }
}

void ball_move(double frameTime)
{
    assert(frameTime < 1.0);
    assert(frameTime >= 0.0);

    if (isStuck) return;

    vec2s scaledVel = glms_vec2_scale(vel, SPEED * frameTime);
    vec2s newPos    = glms_vec2_add(sprite.pos, scaledVel);
    sprite_setPos(&sprite, newPos);
}

void ball_isOob(newPos)
{
    assert(bs);
    assert(pos_new.x > BG_WALL_LEFT - 10.0f);
    assert(pos_new.x < SCR_WIDTH - BG_WALL_RIGHT - BALL_SIZE.s + 10.0f);
    assert(pos_new.y > BG_WALL_TOP - 10.0f);
    assert(pos_new.y < SCR_HEIGHT - BALL_SIZE.s + 10.0f);

    return pos_new.y + bs->size.x > SCR_HEIGHT;
}
