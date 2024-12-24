/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

/* Levels */
#define LVLFOLDER "level"
static const unsigned int lvlcount     = 1;

/* Shader */
static const char vertshader[]      = "shader/sprite_vert.glsl";
static const char fragshader[]      = "shader/sprite_frag.glsl";
static const GLchar projuniform[]   = "proj";
static const GLchar texuniform[]    = "tex";

/* Sprites */
static const char spritefile[]      = "sprite/spritesheet.png";
static const char bgfile[]          = "sprite/background.png";
static const unsigned int brickwidth   = 64;
static const unsigned int brickheight  = 32;
static const unsigned int bricktypes   = 6;   /* Number of brick types */
static const unsigned int ballwidth    = 28;
static const unsigned int ballheight   = 28;
static const unsigned int paddlewidth  = 126;
static const unsigned int paddleheight = 26;
static const unsigned int bgwidth      = 1920;
static const unsigned int bgheight     = 1080;

/* Rigid body simulation */
static const float timestep = 1.0f / 60.0f; /* Simulation update frequency */
static const unsigned int maxsteps = 5;     /* Max steps per frame */
static const unsigned int wallwidth = 64;   /* Walls left, top and right */

/* Key bindings */
static const Key keys[] = {
    { GLFW_KEY_A,     movepaddleleft  },
    { GLFW_KEY_D,     movepaddleright },
    { GLFW_KEY_SPACE, releaseball     }
};

/* Vertices mapping out sprites in spritesheet */
static const unsigned int ballverts[ARRAYCOUNT] = {
    510, 1052,
    537, 1052,
    510, 1079,
    537, 1079
};
static const unsigned int paddleverts[ARRAYCOUNT] = {
    384, 1054,
    509, 1054,
    384, 1079,
    509, 1079
};
static const unsigned int bgverts[ARRAYCOUNT] = {
    0,    0,
    1919, 0,
    0,    1079,
    1919, 1079
};
static const unsigned int brickverts[][ARRAYCOUNT] = {
    /* blue, breakable, id = 0 */
    {
        0,  1048,
        63, 1048,
        0,  1079,
        63, 1079
    },
    /* green, breakable, id = 1 */
    {
        64,  1048,
        127, 1048,
        64,  1079,
        127, 1079
    },
    /* orange, breakable, id = 2 */
    {
        128, 1048,
        191, 1048,
        128, 1079,
        191, 1079
    },
    /* purple, breakable, id = 3 */
    {
        192, 1048,
        255, 1048,
        192, 1079,
        255, 1079
    },
    /* red, breakable, id = 4 */
    {
        256, 1048,
        319, 1048,
        256, 1079,
        319, 1079
    },
    /* yellow, breakable, id = 5 */
    {
        320, 1048,
        383, 1048,
        320, 1079,
        383, 1079
    },
    /* blue, unbreakable, id = a */
    {
        0,  1016,
        63, 1016,
        0,  1047,
        63, 1047
    },
    /* green, unbreakable, id = b */
    {
        64,  1016,
        127, 1016,
        64,  1047,
        127, 1047
    },
    /* orange, unbreakable, id = c */
    {
        128, 1016,
        191, 1016,
        128, 1047,
        191, 1047
    },
    /* purple, unbreakable, id = d */
    {
        192, 1016,
        255, 1016,
        192, 1047,
        255, 1047
    },
    /* red, unbreakable, id = e */
    {
        256, 1016,
        319, 1016,
        256, 1047,
        319, 1047
    },
    /* yellow, unbreakable, id = f */
    {
        320, 1016,
        383, 1016,
        320, 1047,
        383, 1047
    }
};
