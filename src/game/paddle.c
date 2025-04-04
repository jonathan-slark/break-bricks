#include <cglm/struct.h> // vec2s

#include "../main.h"
#include "../gfx/sprite.h"
#include "paddle.h"

// Constants
static const vec2s SIZE        = {{ 144, 24 }};
static const vec2s TEX_OFFSET  = {{ 384, 0 }};
static const vec2s LIVES_POS[] = { {{ 8, 1053 }}, {{ 8, 1027 }} };
static const int   LIVES       = 3;

// Variables
static Sprite  sprite;
static int     score;
static int     lives;
static Sprite* livesSprites;

// Function declarations

void paddle_init(void)
{
    score = 0;
    lives = LIVES;

    vec2s pos     = {{ main_getMousePos().x, SCR_HEIGHT - SIZE.t }};
    vec2s texSize = (vec2s) {{ SCR_WIDTH, SCR_HEIGHT }};
    sprite        = sprite_create(pos, SIZE, TEX_OFFSET, texSize);

    livesSprites = (Sprite *) malloc((LIVES - 1) * sizeof(Sprite));
    for (int i = 0; i < LIVES - 1; i++)
    {
        livesSprites[i] = sprite_create(LIVES_POS[i], SIZE, TEX_OFFSET, texSize);
    }
}

void paddle_setX(float x)
{
    vec2s newPos = {{ x, sprite.pos.y }};
    sprite_setPos(&sprite, newPos);
}

Sprite paddle_getSprite(void)
{
    return sprite;
}

int paddle_getScore(void)
{
    return score;
}
