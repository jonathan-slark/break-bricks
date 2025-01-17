/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

#define IND_COUNT  2  // Number of indices per vertex
#define VERT_COUNT 4  // Number of vertices per quad

typedef struct {
    vec2s pos;
    vec2s texcoord;
} Vert;

// Textured quad
typedef struct {
    Vert verts[VERT_COUNT];
} Quad;

typedef struct {
    GLuint   name;
    unsigned unit;
    vec2s    size;
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

void     gfx_init(void);
void     gfx_term(void);
void     gfx_resize(int width, int height);

Tex      gfx_tex_load(const char* file);
void     gfx_tex_unload(Tex tex);

Renderer gfx_render_create(size_t count, Tex tex);
void     gfx_render_delete(Renderer* r);
void     gfx_render_begin(Renderer* r);
void     gfx_render_quad(Renderer* r, const Quad* q);
void     gfx_render_end(Renderer* r);

Quad     gfx_quad_create(vec2s pos, vec2s size, vec2s tex_offset, Tex t);
vec2s    gfx_quad_pos(Quad* q);
vec2s    gfx_quad_size(Quad* q);
void     gfx_quad_move(Quad* q, vec2s v);
