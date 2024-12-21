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
static const float ballradius          = 14.0f;
static const vec2s ballvelocity        = {{ 100.f, -350.0f }};

/* Vertices mapping out sprites in spritesheet */
static const unsigned int ballverts[ARRAYCOUNT] = {
    512, 0,
    539, 0,
    512, 27,
    539, 27
};
