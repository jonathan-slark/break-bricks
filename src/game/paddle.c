#include <cglm/struct.h> // vec2s

#include "../main.h"
#include "../gfx/sprite.h"
#include "paddle.h"

// Types
typedef struct
{
    Sprite  sprite;
    int     score;
    int     lives;
    Sprite* livesSprites;
} Paddle;

// Constants
static const vec2s PADDLE_SIZE    = {{ 144, 24 }};
static const vec2s PADDLE_OFFSET  = {{ 384, 0 }};
static const vec2s PADDLE_LIVES[] = { {{ 8, 1053 }}, {{ 8, 1027 }} };
static const int   LIVES          = 3;

// Variables
Paddle paddle;

// Function declarations

void paddle_init()
{
    paddle.score = 0;
    paddle.lives = LIVES;

    vec2s pos = {{ main_getMousePos().x, SCR_HEIGHT - PADDLE_SIZE.t }};
    vec2s texSize = (vec2s) {{ SCR_WIDTH, SCR_HEIGHT }};
    paddle.sprite = sprite_create(pos, PADDLE_SIZE, PADDLE_OFFSET, texSize);

    paddle.livesSprites = (Sprite *) malloc((LIVES - 1) * sizeof(Sprite));
    for (int i = 0; i < LIVES - 1; i++)
    {
        paddle.livesSprites[i] = sprite_create(PADDLE_LIVES[i], PADDLE_SIZE, PADDLE_OFFSET, texSize);
    }
}

void paddle_setX(float x)
{
    vec2s newPos = {{ x, paddle.sprite.pos.y }};
    sprite_setPos(&paddle.sprite, newPos, PADDLE_SIZE);
}

Sprite paddle_getSprite(void)
{
    return paddle.sprite;
}
