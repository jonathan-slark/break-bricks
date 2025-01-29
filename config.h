/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

static const Key KEYS[] = {
    { GLFW_KEY_ESCAPE, quit },
    { GLFW_KEY_SPACE, pause }
};
static const Button BUTTONS[] = {
    { GLFW_MOUSE_BUTTON_LEFT, click }
};

static const char     LOADING_FILE[] = "gfx/loading.png";
static const char     LEVEL_FOLDER[] = "level";
static const unsigned LEVEL_COUNT    = 1;
static const char     HISCORE_FILE[] = "hiscore.txt";
static const unsigned LIVES          = 3;
static const unsigned CR_COUNT       = 2; // How many times per frame to run collision resolution

static const char     FONT_FILE[]    = "font/JupiteroidRegular.ttf";
static const unsigned FONT_HEIGHTS[] = { 64, 40 };
static const Text     TEXT_SCORE     = { FontLarge,  {{ 192,  56  }}, {{ 1.0f, 1.0f, 1.0f }}, "%u" };
static const Text     TEXT_HISCORE   = { FontLarge,  {{ 1573, 56  }}, {{ 1.0f, 1.0f, 1.0f }}, "%u" };
static const Text     TEXT_PAUSED    = { FontLarge,  {{ 880,  600 }}, {{ 1.0f, 1.0f, 1.0f }}, "Paused." };
static const Text     TEXT_MENU      = { FontMedium, {{ 680,  860 }}, {{ 0.6f, 0.6f, 0.6f }},
    "Use the mouse to control the paddle.\n"
    "Click mouse button to release the ball.\n"
    "Press space to pause.\n"
    "Press escape to quit.\n"
    "\n"
    "Click mouse button to continue." };
static const Text     TEXT_LOST       = { FontLarge,  {{ 840, 600 }}, {{ 1.0f, 1.0f, 1.0f }}, "Game over." };
static const Text     TEXT_NEWHISCORE = { FontLarge,  {{ 820, 664 }}, {{ 1.0f, 1.0f, 1.0f }}, "New hiscore!" };
static const Text     TEXT_WON        = { FontLarge,  {{ 855, 600 }}, {{ 1.0f, 1.0f, 1.0f }}, "You won!" };
static const Text     TEXT_CONTINUE   = { FontMedium, {{ 745, 860 }}, {{ 1.0f, 1.0f, 1.0f }},
    "Click mouse button to continue." };

static const float       AUD_VOL     = 0.1; // Volume 0 - 1
static const char        AUD_BRICK[] = "sfx/brick.wav";
static const char        AUD_DEATH[] = "sfx/death.wav";
static const char        AUD_CLEAR[] = "sfx/clear.wav";
static const char        AUD_WON[]   = "sfx/won.wav";
static const char        AUD_LOST[]  = "sfx/lost.wav";
static const char* const AUD_MUSIC[] = {
    "music/HoliznaCC0 - 2nd Dimension.mp3", // One track per level
    "music/HoliznaCC0 - Astroids.mp3",
    "music/HoliznaCC0 - In The End.mp3",
    "music/HoliznaCC0 - Pixel Party.mp3",
    "music/HoliznaCC0 - Scroller.mp3",
    "music/HoliznaCC0 - Space Castle.mp3",
    "music/HoliznaCC0 - Where It's Safe.mp3"
};

static const char     BG_FILE[]      = "gfx/background.png";
static const unsigned BG_WALL_TOP    = 72;
static const unsigned BG_WALL_LEFT   = 192;
static const unsigned BG_WALL_RIGHT  = 192;

/*
 * Sprites
 */

static const char SPRITE_SHEET[] = "gfx/spritesheet.png";

static const vec2s PADDLE_SIZE    = {{ 144, 24 }};
static const vec2s PADDLE_OFFSET  = {{ 384, 0 }};
static const vec2s PADDLE_LIVES[] = {
    {{ 8, 1053 }},
    {{ 8, 1027 }}
};

static const vec2s    BALL_SIZE       = {{ 24, 24 }};
static const vec2s    BALL_OFFSET     = {{ 528, 0 }};
static const unsigned BALL_SPEED      = 750;  // Pixels per second
static const float    BALL_BOUNCE_STR = 1.5f; // Strength of deflect off paddle
// Choose a random release vector for the ball
static const vec2s    BALL_RELEASE[] = {
    {{ -0.5f, -0.5f }},
    {{  0.5f, -0.5f }}
};

static const vec2s    BRICK_SIZE  = {{ 128, 32 }};
static const unsigned BRICK_COLS  = 12;
static const unsigned BRICK_ROWS  = 12;
static const vec2s    BRICK_SINGLE_OFFSETS[] = {
    {{ 0,   64 }}, // blue,   id = 0
    {{ 128, 64 }}, // green,  id = 1
    {{ 256, 64 }}, // orange, id = 2
    {{ 0,   96 }}, // purple, id = 3
    {{ 128, 96 }}, // red,    id = 4
    {{ 256, 96 }}  // yellow, id = 5
};
static const vec2s    BRICK_SOLID_OFFSETS[] = {
    {{ 0,   0 }},  // blue,   id = a
    {{ 128, 0 }},  // green,  id = b
    {{ 256, 0 }},  // orange, id = c
    {{ 0,   32 }}, // purple, id = d
    {{ 128, 32 }}, // red,    id = e
    {{ 256, 32 }}, // yellow, id = f
};
