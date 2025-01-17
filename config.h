/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

// Key bindings
static const Key KEYS[] = {
    { GLFW_KEY_ESCAPE, quit },
    { GLFW_KEY_SPACE, pause }
};
static const Button BUTTONS[] = {
    { GLFW_MOUSE_BUTTON_LEFT, releaseball }
};

// Levels
static const char     LVL_FOLDER[] = "level";
static const unsigned LVL_COUNT    = 2;

// Sprites
static const char     SPRITE_SHEET[]  = "gfx/spritesheet.png";
static const char     BACKGROUND[]    = "gfx/background.png";
static const unsigned WALL_WIDTH      = 32;   // Walls left, top and right
static const vec2s    BALL_SIZE       = {{ 28, 28 }};
static const unsigned BALL_MOVE       = 750;  // Pixels per second
static const float    BALL_BOUNCE_STR = 1.5f; // Strength of deflect off paddle
static const vec2s    PADDLE_SIZE     = {{ 126, 26 }};
static const vec2s    BRICK_SIZE      = {{ 64,  32 }};
static const unsigned BRICK_COLS      = 29;
static const unsigned BRICK_ROWS      = 12;
static const unsigned BRICK_TYPES     = 6;    // Number of brick types

// Sounds
static const float AUD_VOL     = 0.2; // Volume 0 - 1
static const char  AUD_BRICK[] = "sfx/brick.wav";
static const char  AUD_DEATH[] = "sfx/death.wav";
static const char  AUD_WIN[]   = "sfx/win.wav";
static const char  AUD_MUSIC[] = "music/HoliznaCC0 - 2nd Dimension.mp3";

// Game logic
// How many times per frame to run collision resolution
static const unsigned RES_COUNT = 2;
// Choose a random release vector for the ball
static const vec2s BALL_RELEASE[] = {
    {{ -0.5f, -0.5f }},
    {{  0.5f, -0.5f }}
};

// Vertices mapping out sprites in spritesheet
static const vec2s BALL_OFFSET   = {{ 510, 0 }};
static const vec2s PADDLE_OFFSET = {{ 384, 0 }};
static const vec2s BRICK_OFFSETS[] = {
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
