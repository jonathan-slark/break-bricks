#pragma once
#undef GLAD_GL_IMPLEMENTATION

#include <glad.h>
#include <stdlib.h> // size_t

#include "quad.h"

// Types
typedef struct {
    // Vertex buffer data
    GLuint    vao, vbo, ebo;
    size_t    vertCount;
    size_t    vertMax;
    Vert*     verts;
    GLushort* indices;

    // One texture per renderer to minimise state changes
    Tex tex;
} Rend;

// Function prototypes
Rend render_create(size_t count, const char* file);
void render_delete(Rend* r);
void render_begin(Rend* r);
void render_quad(Rend* r, const Quad* q);
void render_end(Rend* r);
