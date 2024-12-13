/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

static const char title[]             = "Break Bricks";
static const unsigned int scrwidth    = 800;
static const unsigned int scrheight   = 600;
static const unsigned int openglmajor = 4;
static const unsigned int openglminor = 6;

static const char shader_vertex[]   = "shader/sprite_vert.spv";
static const char shader_fragment[] = "shader/sprite_frag.spv";
static const GLint shader_modelloc  = 0;
static const GLint shader_projloc   = 1;
static const GLint shader_texloc    = 2;

static const char sprite_brick[]    = "tex/brick.png";
static const char sprite_solid[]    = "tex/solid.png";
