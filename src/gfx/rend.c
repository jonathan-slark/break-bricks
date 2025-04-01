#include <stdlib.h> // size_t

#include "../util.h"
#include "gfx.h"
#include "rend.h"
#include "shader.h"
#include "tex.h"

// Constants
static const GLushort QUAD_INDICES[] = { 0, 1, 2, 0, 2, 3 };

// Function declarations

Rend rend_create(size_t count)
{
    Rend r;
    r.vertMax      = count * VERT_COUNT;
    size_t qiCount = COUNT(QUAD_INDICES);
    size_t viCount  = count * qiCount;
    size_t viSize   = sizeof(GLushort) * viCount;

    // Pre-calculate the entire index buffer
    r.indices = (GLushort*) malloc(viSize);
    for (size_t i = 0; i < viCount; i++)
    {
        r.indices[i] = i / qiCount * VERT_COUNT + QUAD_INDICES[i % qiCount];
    }

    glGenVertexArrays(1, &r.vao);
    glBindVertexArray(r.vao);

    glGenBuffers(1, &r.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, r.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vert) * r.vertMax, NULL, GL_DYNAMIC_DRAW);

    glGenBuffers(1, &r.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, viSize, r.indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, IND_COUNT, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*) offsetof(Vert, pos));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, IND_COUNT, GL_FLOAT, GL_FALSE, sizeof(Vert), (void*) offsetof(Vert, texCoord));

    r.vertCount = 0;
    r.verts = (Vert*) malloc(sizeof(Vert) * r.vertMax);

    return r;
}

Rend rend_load(size_t count, const char* file)
{
    Rend r = rend_create(count);
    r.tex = tex_load(file);
    return r;
}

void rend_unload(Rend r)
{
    tex_unload(r.tex);
    if (r.verts)   free(r.verts);
    if (r.indices) free(r.indices);
    glDeleteBuffers(1, &r.ebo);
    glDeleteBuffers(1, &r.vbo);
    glDeleteVertexArrays(1, &r.vao);
}

void rend_begin(Rend r)
{
    Shader s = gfx_getShader();
    shader_setTex(s, r.tex.unit);
    shader_setCol(s, (vec3s) {{ 1.0f, 1.0f, 1.0f }});
    shader_setIsFont(s, false);
}

void flush(Rend* r)
{
    if (!r->vertCount) return;

    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vert) * r->vertCount,
            r->verts);

    glBindVertexArray(r->vao);
    GLsizei count = r->vertCount / VERT_COUNT * COUNT(QUAD_INDICES);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);

    r->vertCount = 0;
}

void rend_end(Rend* r)
{
    flush(r);
}

void rend_sprite(Rend* r, Sprite s)
{
    if (r->vertCount == r->vertMax)
    {
#ifndef NDEBUG
        fprintf(stderr, "%s\n", "Warning: flushed full vertex cache.");
#endif // !NDEBUG
        flush(r);
    }

    for (size_t i = 0; i < VERT_COUNT; i++)
    {
        r->verts[r->vertCount++] = s.verts[i];
    }
}
