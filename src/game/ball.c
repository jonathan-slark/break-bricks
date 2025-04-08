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

#include <math.h>
#include <float.h> // for INFINITY

// Swept AABB collision detection function. 'moving' is the ball, 'movement'
// is its full movement vector for the frame, 'target' is the static object
// (paddle, brick, or wall), and 'normal' will be set to the collision normal.
// Returns a collision time in [0.0, 1.0]. If 1.0 is returned, no collision
// occurred.
float sweptAABB(Sprite moving, vec2s movement, Sprite target, vec2s* normal)
{
    float xInvEntry, yInvEntry; float xInvExit, yInvExit;
    if (movement.x > 0.0f) {
	xInvEntry = target.pos.x - (moving.pos.x + moving.size.s);
	xInvExit  = (target.pos.x + target.size.s) - moving.pos.x;
    } else {
	xInvEntry = (target.pos.x + target.size.s) - moving.pos.x;
	xInvExit  = target.pos.x - (moving.pos.x + moving.size.s);
    }

    if (movement.y > 0.0f) {
	yInvEntry = target.pos.y - (moving.pos.y + moving.size.t);
	yInvExit  = (target.pos.y + target.size.t) - moving.pos.y;
    } else {
	yInvEntry = (target.pos.y + target.size.t) - moving.pos.y;
	yInvExit  = target.pos.y - (moving.pos.y + moving.size.t);
    }

    float xEntry, yEntry;
    float xExit,  yExit;

    if (movement.x == 0.0f) {
	xEntry = -INFINITY;
	xExit  = INFINITY;
    } else {
	xEntry = xInvEntry / movement.x;
	xExit  = xInvExit / movement.x;
    }

    if (movement.y == 0.0f) {
	yEntry = -INFINITY;
	yExit  = INFINITY;
    } else {
	yEntry = yInvEntry / movement.y;
	yExit  = yInvExit / movement.y;
    }

    float entryTime = fmaxf(xEntry, yEntry);
    float exitTime  = fminf(xExit, yExit);

    // No collision if there is no overlap during the movement
    if (entryTime > exitTime || (xEntry < 0.0f && yEntry < 0.0f) || entryTime > 1.0f) {
	normal->x = 0.0f;
	normal->y = 0.0f;
	return 1.0f;
    } else {
	// Determine the collision normal based on which axis had the later entry
	if (xEntry > yEntry) {
	    if (xInvEntry < 0.0f) {
		normal->x = 1.0f;
		normal->y = 0.0f;
	    } else {
		normal->x = -1.0f;
		normal->y = 0.0f;
	    }
	} else {
	    if (yInvEntry < 0.0f) {
		normal->x = 0.0f;
		normal->y = 1.0f;
	    } else {
		normal->x = 0.0f;
		normal->y = -1.0f;
	    }
	}
	return entryTime;
    }
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

	// Check collision with the paddle
	vec2s tempNormal;
	float t = sweptAABB(ball, movement, paddle_getSprite(), &tempNormal);
	if (t < earliestCollisionTime) {
	    earliestCollisionTime = t;
	    collisionNormal = tempNormal;
	}
	// Walls
	for (Wall i = 0; i < WallCount; i++)
	{
	    t = sweptAABB(ball, movement, wall_getSprite(i), &tempNormal);
	    if (t < earliestCollisionTime) { earliestCollisionTime = t; collisionNormal = tempNormal; }
	}

	// Move the ball up to the collision point.
	vec2s movePart = glms_vec2_scale(movement, earliestCollisionTime);
	sprite_setPos(&ball, glms_vec2_add(ball.pos, movePart));

	// If no collision occurred during this movement, we're done.
	if (earliestCollisionTime == 1.0f) {
	    break;
	}

	// Reflect the ball's velocity off the collider.
	vel = glms_vec2_reflect(vel, collisionNormal);

	// Deduct the used portion of the frame time.
	remainingTime *= (1.0f - earliestCollisionTime);
	iter++;
    }
}
