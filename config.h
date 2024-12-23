/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

static const char vertshader[]      = "sprite_vert.glsl";
static const char fragshader[]      = "sprite_frag.glsl";
static const GLchar projuniform[]   = "proj";
static const GLchar texuniform[]    = "tex";

static const char spritefile[]      = "spritesheet.png";
static const unsigned int brickwidth   = 64;
static const unsigned int brickheight  = 32;
static const unsigned int bricktypes   = 6;     /* Number of brick types */
static const unsigned int ballwidth    = 28;
static const unsigned int ballheight   = 28;
static const unsigned int paddlewidth  = 126;
static const unsigned int paddleheight = 26;
static const unsigned int lvlcount     = 1;

/* Vertices mapping out sprites in spritesheet */
static const unsigned int ballverts[ARRAYCOUNT] = {
    510, 0,
    537, 0,
    510, 27,
    537, 27
};
static const unsigned int paddleverts[ARRAYCOUNT] = {
    384, 0,
    509, 0,
    384, 25,
    509, 25
};
static const unsigned int brickverts[][ARRAYCOUNT] = {
    /* blue, breakable, id = 0 */
    {
        0,  0,
        63, 0,
        0,  31,
        63, 31
    },
    /* green, breakable, id = 1 */
    {
        64,  0,
        127, 0,
        64,  31,
        127, 31
    },
    /* orange, breakable, id = 2 */
    {
        128, 0,
        191, 0,
        128, 31,
        191, 31
    },
    /* purple, breakable, id = 3 */
    {
        192, 0,
        255, 0,
        192, 31,
        255, 31
    },
    /* red, breakable, id = 4 */
    {
        256, 0,
        319, 0,
        256, 31,
        319, 31
    },
    /* yellow, breakable, id = 5 */
    {
        320, 0,
        383, 0,
        320, 31,
        383, 31
    },
    /* blue, unbreakable, id = a */
    {
        0,  32,
        63, 32,
        0,  63,
        63, 63
    },
    /* green, unbreakable, id = b */
    {
        64,  32,
        127, 32,
        64,  63,
        127, 63
    },
    /* orange, unbreakable, id = c */
    {
        128, 32,
        191, 32,
        128, 63,
        191, 63
    },
    /* purple, unbreakable, id = d */
    {
        192, 32,
        255, 32,
        192, 63,
        255, 63
    },
    /* red, unbreakable, id = e */
    {
        256, 32,
        319, 32,
        256, 63,
        319, 63
    },
    /* yellow, unbreakable, id = f */
    {
        320, 32,
        383, 32,
        320, 63,
        383, 63
    }
};
