/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

static const char title[]             = "Break Bricks";
static const unsigned int scrwidth    = 320;
static const unsigned int scrheight   = 200;
static const unsigned int scrscale    = 2;
static const unsigned int openglmajor = 3;
static const unsigned int openglminor = 3;

static const char vertshader[]     = "shader/sprite_vert.glsl";
static const char fragshader[]     = "shader/sprite_frag.glsl";
static const GLchar modeluniform[] = "model";
static const GLchar projuniform[]  = "proj";
static const GLchar texuniform[]   = "tex";
static const char spritefile[]     = "tex/spritesheet.png";

static const unsigned int lvlwidth  = 10;
static const unsigned int lvlheight = 3;
static const unsigned int lvlcount  = 1;
static const char * const levels[] = {
    "level/01.txt"
};
