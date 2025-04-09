#include <cglm/struct.h> // vec2s

#include "../main.h"
#include "../gfx/rend.h"
#include "../gfx/sprite.h"
#include "paddle.h"

// Constants
static const vec2s SIZE        = {{ 144, 24 }};
static const vec2s TEX_OFFSET  = {{ 384, 0 }};
static const vec2s LIVES_POS[] = { {{ 8, 1053 }}, {{ 8, 1027 }} };
static const int   LIVES       = 3;

// Variables
static Sprite  paddle;
static int     score;
static int     lives;
static Sprite* livesSprites;

// Function definitions

void paddle_resetStats(void)
{
    score = 0;
    lives = LIVES;
}

void paddle_init(void)
{
    paddle_resetStats();

    vec2s pos     = {{ main_getMousePos().x, SCR_HEIGHT - SIZE.t }};
    vec2s texSize = (vec2s) {{ SCR_WIDTH, SCR_HEIGHT }};
    paddle        = sprite_create(pos, SIZE, TEX_OFFSET, texSize);

    livesSprites = (Sprite *) malloc((LIVES - 1) * sizeof(Sprite));
    for (int i = 0; i < LIVES - 1; i++) {
        livesSprites[i] = sprite_create(LIVES_POS[i], SIZE, TEX_OFFSET, texSize);
    }
}

void paddle_setX(float x)
{
    vec2s newPos = {{ x, paddle.pos.y }};
    sprite_setPos(&paddle, newPos);
}

Sprite paddle_getSprite(void)
{
    return paddle;
}

void paddle_rend(Rend* r)
{
    rend_sprite(r, paddle);
    for (int i = 0; i < lives - 1; i++) {
	rend_sprite(r, livesSprites[i]);
    }
}

int paddle_getScore(void)
{
    return score;
}

void paddle_incScore(int s)
{
    score += s;
}

// Return false if game over
bool paddle_lifeLost(void)
{
    return --lives != 0;
}
