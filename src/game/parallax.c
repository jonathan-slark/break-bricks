#include "../main.h"
#include "../util.h"
#include "../gfx/rend.h"
#include "../gfx/sprite.h"
#include "paddle.h"
#include "parallax.h"

// Constants
static const char* FILES[] =  { "gfx/stars1.png", "gfx/stars2.png" };
static const float RATIOS[] = { 0.000015f, 0.00002f };
constexpr size_t COUNT = COUNT(FILES);

// Variables
static Rend   rends[COUNT];
static Sprite sprites[COUNT];
static float  paddlePrevX;

void parallax_load(void)
{
    for (size_t i = 0; i < COUNT; i++) {
	rends[i]   = rend_load(1, FILES[i]);

	vec2s pos    = {{ 0, 0 }};
	vec2s texPos = {{ 500, 500 }};
	vec2s size   = {{ SCR_WIDTH, SCR_HEIGHT }};
	sprites[i] = sprite_create(pos, size, texPos, rends[i].tex.size);
    }

    paddlePrevX = paddle_getSprite().pos.x;
}

void parallax_unload(void)
{
    for (size_t i = 0; i < COUNT; i++) rend_unload(rends[i]);
}

void parallax_onPaddleMove(void)
{
    Sprite ps = paddle_getSprite();
    float paddleDeltaX = ps.pos.x - paddlePrevX;
    paddlePrevX = ps.pos.x;

    for (size_t i = 0; i < COUNT; i++) {
	sprite_texOffAdd(&sprites[i], (vec2s) {{ paddleDeltaX * RATIOS[i], 0.0f }});
    }
}

void parallax_rend(void)
{
    for (size_t i = 0; i < COUNT; i++) {
	rend_begin(rends[i]);
	rend_sprite(&rends[i], sprites[i]);
	rend_end(&rends[i]);
    }
}
