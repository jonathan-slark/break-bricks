/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

static const Key KEYS[] = {
    { GLFW_KEY_ESCAPE, quit },
};

static const char BACKGROUND[]   = "gfx/background.png";
static const char SPRITE_SHEET[] = "gfx/spritesheet.png";

static const unsigned WALL_WIDTH = 32; // Walls left, top and right

static const vec2s PADDLE_SIZE   = {{ 126, 26 }};
static const vec2s PADDLE_OFFSET = {{ 384, 0 }};
