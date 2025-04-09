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
    if (isStuck) {
	sprite_setPos(&ball, getStuckPos());
    }
}

void ball_release(void)
{
    if (isStuck) {
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

    float remainingTime = 1.0f;   // Represents the fraction of the frame left to move.
    const int MAX_ITERATIONS = 4; // Prevent infinite loops with multiple collisions.
    int iter = 0;

    // Continue moving until we've used the full frame time or hit the iteration limit.
    while (remainingTime > 0.0f && iter < MAX_ITERATIONS) {
	// Calculate the full movement vector for the remaining frame time.
	vec2s movement = glms_vec2_scale(vel, SPEED * frameTime * remainingTime);

	float earliestCollisionTime = 1.0f;
	vec2s collisionNormal = {{0, 0}};

	// Check collision with the paddle.
	vec2s tempNormal;
	bool paddleHit = false;
	float t = sprite_sweptAABB(ball, movement, paddle_getSprite(), &tempNormal);
	if (t < earliestCollisionTime) {
	    earliestCollisionTime = t;
	    collisionNormal = tempNormal;
	    paddleHit = true;
	}
	// Walls.
	for (Wall i = 0; i < WallCount; i++)
	{
	    t = sprite_sweptAABB(ball, movement, wall_getSprite(i), &tempNormal);
	    if (t < earliestCollisionTime) {
		earliestCollisionTime = t;
		collisionNormal = tempNormal;
		paddleHit = false;
	    }
	}
	// Bricks.
	int brickHit = -1;
	for (int i = 0; i < level_getBrickCount(); i++) {
	    Sprite* brick = level_getBrickSprite(i);
	    if (brick) {
		t = sprite_sweptAABB(ball, movement, *brick, &tempNormal);
		if (t < earliestCollisionTime) {
		    earliestCollisionTime = t;
		    collisionNormal = tempNormal;
		    brickHit = i;
		    paddleHit = false;
		}
	    }
	}

	// Move the ball up to the collision point.
	vec2s movePart = glms_vec2_scale(movement, earliestCollisionTime);
	sprite_setPos(&ball, glms_vec2_add(ball.pos, movePart));

	// If ball is OOB then lose a life and check for game over.
	if (isOob()) {
	    ball_init();
	    if (paddle_lifeLost()) {
		audio_playSound(SoundDeath);
	    } else {
		game_lost();
		return;
	    }
	}

	// If no collision occurred during this movement, we're done.
	if (earliestCollisionTime == 1.0f) break;

	// Reflect the ball's velocity off the collider.
	vel = glms_vec2_reflect(vel, collisionNormal);

	// Modify X velocity based on where it hit the paddle, unless it bounced off the side.
	if (paddleHit && vel.y < 0.0f) {
	    Sprite paddle = paddle_getSprite();
	    float paddleCenter = paddle.pos.x + paddle.size.x / 2.0f;
	    float hitOffset    = (ball.pos.x + ball.size.x / 2.0f) - paddleCenter;
	    vel.x = hitOffset / (paddle.size.x / 2.0f);
	    vel   = glms_vec2_normalize(vel);
	}

	// If collision was with a brick then destroy it.
	if (brickHit > -1) level_destroyBrick(brickHit);

	// Deduct the used portion of the frame time.
	remainingTime *= (1.0f - earliestCollisionTime);
	iter++;
    }
}
