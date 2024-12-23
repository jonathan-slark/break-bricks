/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

static const char vertshader[]      = "sprite_vert.glsl";
static const char fragshader[]      = "sprite_frag.glsl";
static const GLchar projuniform[]   = "proj";
static const GLchar texuniform[]    = "tex";

static const char spritefile[]      = "spritesheet.png";
static const unsigned int ballwidth    = 28;
static const unsigned int ballheight   = 28;
static const unsigned int paddlewidth  = 126;
static const unsigned int paddleheight = 26;

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
