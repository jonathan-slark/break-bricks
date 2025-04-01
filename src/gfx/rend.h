#pragma once
#undef GLAD_GL_IMPLEMENTATION

#include <glad.h>   // GL*
#include <stdlib.h> // size_t

#include "sprite.h"
#include "tex.h"

// Types
typedef struct
{
    // Vertex buffer data
    GLuint    vao;
    GLuint    vbo;
    GLuint    ebo;
    size_t    vertCount;
    size_t    vertMax;
    Vert*     verts;
    GLushort* indices;

    // One texture per renderer to minimise state changes
    Tex tex;
} Rend;

// Function prototypes
Rend rend_create(size_t count);
Rend rend_load(size_t count, const char* file);
void rend_unload(Rend r);
void rend_begin(Rend r);
void rend_sprite(Rend* r, Sprite s);
void rend_end(Rend* r);
