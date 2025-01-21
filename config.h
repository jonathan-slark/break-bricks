/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

static const Key KEYS[] = {
    { GLFW_KEY_ESCAPE, quit },
    { GLFW_KEY_SPACE, pause }
};
static const Button BUTTONS[] = {
    { GLFW_MOUSE_BUTTON_LEFT, ball_release }
};

static const char     LEVEL_FOLDER[] = "level";
static const unsigned LEVEL_COUNT    = 2;
// How many times per frame to run collision resolution
static const unsigned CR_COUNT = 2;

static const char     BG_FILE[] = "gfx/background.png";
static const vec2s    BG_SIZE   = {{ 1920, 1080 }};
static const vec2s    BG_OFFSET = {{ 0, 0 }};
static const unsigned BG_WALL   = 32; // Walls left, top and right

static const char SPRITE_SHEET[] = "gfx/spritesheet.png";

static const vec2s PADDLE_SIZE       = {{ 126, 26 }};
static const vec2s PADDLE_OFFSET     = {{ 384, 0 }};
// Collision detection: the paddle is a pill shape
static const vec2s PADDLE_CIRCLE1POS = {{ 13,  13 }};
static const vec2s PADDLE_CIRCLE2POS = {{ 113, 13 }};
static const vec2s PADDLE_AABB[]     = { {{ 13, 0 }}, {{ 113, 25 }} };

static const vec2s    BALL_SIZE       = {{ 28, 28 }};
static const vec2s    BALL_OFFSET     = {{ 510, 0 }};
static const unsigned BALL_SPEED      = 750; // Pixesl per second
static const float    BALL_BOUNCE_STR = 1.5f; // Strength of deflect off paddle
// Choose a random release vector for the ball
static const vec2s    BALL_RELEASE[] = {
    {{ -0.5f, -0.5f }},
    {{  0.5f, -0.5f }}
};

static const vec2s    BRICK_SIZE  = {{ 64,  32 }};
static const unsigned BRICK_COLS  = 29;
static const unsigned BRICK_ROWS  = 12;
static const unsigned BRICK_TYPES = 6; // Number of brick colours
static const vec2s    BRICK_OFFSETS[] = {
    {{ 0,   0  }}, // blue, breakable, id = 0
    {{ 64,  0  }}, // green, breakable, id = 1
    {{ 128, 0  }}, // orange, breakable, id = 2
    {{ 192, 0  }}, // purple, breakable, id = 3
    {{ 256, 0  }}, // red, breakable, id = 4
    {{ 320, 0  }}, // yellow, breakable, id = 5
    {{ 0,   32 }}, // blue, unbreakable, id = a
    {{ 64,  32 }}, // green, unbreakable, id = b
    {{ 128, 32 }}, // orange, unbreakable, id = c
    {{ 192, 32 }}, // purple, unbreakable, id = d
    {{ 256, 32 }}, // red, unbreakable, id = e
    {{ 320, 32 }}  // yellow, unbreakable, id = f
};
