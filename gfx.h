/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

#define INDCOUNT   2  // Number of indices per vertex
#define VERTCOUNT  4  // Number of vertices per sprite
#define ARRAYCOUNT (INDCOUNT * VERTCOUNT)

// Data pushed to shader, per vertex
typedef struct {
    GLfloat pos[INDCOUNT];
    GLfloat texcoord[INDCOUNT];
} Vert;

typedef struct {
    GLuint id;
    int width, height;
} Tex;

typedef struct {
    // Vertex buffer data
    GLuint    vao, vbo, ebo;
    size_t    vert_count;
    size_t    vert_max;
    Vert*     verts;
    GLushort* indices;

    // Uniforms
    GLuint tex;
} Renderer;

void     gfx_init(void);
void     gfx_term(void);
void     gfx_resize(int width, int height);

Tex      gfx_tex_load(const char* file);
void     gfx_tex_unload(Tex tex);

Renderer gfx_render_create(size_t count, Tex tex);
void     gfx_render_delete(Renderer* r);
void     gfx_render_flush(Renderer* r);
void     gfx_render_push(Renderer* r, Sprite* s);
