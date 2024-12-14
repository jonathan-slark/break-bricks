/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

static const char title[]             = "Break Bricks";
static const unsigned int scrwidth    = 320;
static const unsigned int scrheight   = 200;
static const unsigned int openglmajor = 4;
static const unsigned int openglminor = 6;

static const char vertshader[] = "shader/sprite_vert.spv";
static const char fragshader[] = "shader/sprite_frag.spv";
static const GLint modelloc    = 0;
static const GLint projloc     = 1;
static const GLint texloc      = 2;
static const char spritefile[] = "tex/spritesheet.png";
