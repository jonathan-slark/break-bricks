/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 * TODO: directly initialise structs here?
*/

static const char title[]     = "Break Bricks";
static const int scrwidth     = 1920;
static const int scrheight    = 1080;
static const int scrredbits   = 8;
static const int scrgreenbits = 8;
static const int scrbluebits  = 8;
static const int openglmajor  = 3;
static const int openglminor  = 3;

static const char vertshader[]      = "shader/sprite_vert.glsl";
static const char fragshader[]      = "shader/sprite_frag.glsl";
static const GLchar modeluniform[]  = "model";
static const GLchar projuniform[]   = "proj";
static const GLchar texuniform[]    = "tex";
static const char spritefile[]      = "tex/spritesheet.png";

static const unsigned int brickwidth   = 64;
static const unsigned int brickheight  = 32;
static const unsigned int bricktypes   = 6;
static const unsigned int paddlewidth  = 128;
static const unsigned int paddleheight = 28;
static const float paddlevelocity      = 500.0f;
static const unsigned int ballwidth    = 28;
static const unsigned int ballheight   = 28;
static const float ballradius          = 14.0f;
static const vec2s ballvelocity        = {{ 100.f, -350.0f }};
static const unsigned int lvlcount     = 1;
static const char lvlfolder[]          = "level/01.txt";
