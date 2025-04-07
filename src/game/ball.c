#include <cglm/struct.h> // vec2s

#include "../main.h"
#include "../util.h"
#include "../gfx/sprite.h"
#include "audio.h"
#include "ball.h"
#include "game.h"
#include "paddle.h"
#include "level.h"
#include "wall.h"

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
static Sprite ball;

// Function definitions

vec2s getStuckPos(void)
{
    Sprite ps = paddle_getSprite();
    return (vec2s) {{ ps.pos.x + ps.size.s / 2.0f - SIZE.s / 2.0f, ps.pos.y - SIZE.t }};
}

void ball_init(void)
{
    isStuck = true;
    ball    = sprite_create(getStuckPos(), SIZE, TEX_OFFSET, (vec2s) {{ SCR_WIDTH, SCR_HEIGHT }} );
}

void ball_rend(Rend* r)
{
    rend_sprite(r, ball);
}

void ball_onPaddleMove(void)
{
    if (isStuck)
    {
	sprite_setPos(&ball, getStuckPos());
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

bool isOob(void)
{
    return ball.pos.y + ball.size.t >= SCR_HEIGHT;
}

void ball_move(double frameTime)
{
    if (isStuck) return;

    vec2s scaledVel = glms_vec2_scale(vel, SPEED * frameTime);
    vec2s newPos    = glms_vec2_add(ball.pos, scaledVel);
    sprite_setPos(&ball, newPos);

    // If ball is OOB then lose a life and check for game over
    if (isOob())
    {
	ball_init();
	if (paddle_lifeLost())
	{
	    audio_playSound(SoundDeath);
	}
	else
	{
	    game_lost();
	    return;
	}
    }

    // Wall bounce
    if (wall_isCollisionX(ball)) vel.x *= -1.0f;
    if (wall_isCollisionY(ball)) vel.y *= -1.0f;

    // Paddle bounce
    Sprite paddle = paddle_getSprite();
    if (sprite_checkCollision(ball, paddle)) {
	vel.y *= -1.0f;

	// Modify X velocity based on where it hit the paddle
	float paddleCenter = paddle.pos.x + paddle.size.x / 2.0f;
	float hitOffset    = (ball.pos.x + ball.size.x / 2.0f) - paddleCenter;
	vel.x = hitOffset / (paddle.size.x / 2.0f);
	vel   = glms_vec2_normalize(vel);
    }

    // Brick bounce and destruction
    vec2s normal;
    if (level_checkCollision(ball, &normal))
    {
	vel = glms_vec2_reflect(vel, normal);
    }
}
