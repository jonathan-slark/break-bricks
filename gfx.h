/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

#undef GLAD_GL_IMPLEMENTATION
#undef STB_TRUETYPE_IMPLEMENTATION
#include <cglm/struct.h>
#include <glad.h>
#include <stb/stb_truetype.h>

// Graphics subsystem

void gfx_init(void);
void gfx_term(void);
void gfx_resize(int width, int height);

// Quad renderer

#define IND_COUNT  2  // Number of indices per vertex
#define VERT_COUNT 4  // Number of vertices per quad

typedef struct {
    vec2s pos;
    vec2s tex_coord;
} Vert;

typedef struct {
    Vert verts[VERT_COUNT];
} Quad;

typedef struct {
    GLuint name;
    GLenum unit;
    vec2s  size;
} Tex;

typedef struct {
    // Vertex buffer data
    GLuint    vao, vbo, ebo;
    size_t    vert_count;
    size_t    vert_max;
    Vert*     verts;
    GLushort* indices;

    // One texture per renderer to minimise state changes
    Tex tex;
} Renderer;

Renderer gfx_render_create(size_t count, const char* file);
void     gfx_render_delete(Renderer* r);
void     gfx_render_begin(Renderer* r);
void     gfx_render_quad(Renderer* r, const Quad* q);
void     gfx_render_end(Renderer* r);

Quad     gfx_quad_create(Renderer* r, vec2s pos, vec2s size, vec2s tex_offset);
void     gfx_quad_set_pos(Quad* q, vec2s pos, vec2s size);
void     gfx_quad_add_vec(Quad* q, vec2s v);

// Font renderer

#define ASCII_FIRST 32
#define ASCII_LAST  126
#define ASCII_COUNT (ASCII_LAST + 1 - ASCII_FIRST)

typedef struct {
    Renderer render;
    stbtt_packedchar chars[ASCII_COUNT];
} Font;

Font gfx_font_create(unsigned height, const char* file);
void gfx_font_delete(Font* f);
void gfx_font_begin(Font* f);
void gfx_font_printf(Font* f, vec2s pos, vec3s colour, const char* fmt, ...);
void gfx_font_end(Font* f);
