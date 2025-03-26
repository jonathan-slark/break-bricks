#include <stdlib.h> // size_t

#include "rend.h"
#include "tex.h"
#include "util.h"

// Function prototypes
static Rend create(size_t count);

// Function declarations

Rend create(size_t count) {
    Rend r;
    r.vertMax      = count * VERT_COUNT;
    size_t qiCount = COUNT(QUAD_INDICES);
    size_t iCount  = count * qiCount;

    // Pre-calculate the entire index buffer
    r.indices = (GLushort*) malloc(sizeof(GLushort) * iCount);
    for (size_t i = 0; i < iCount; i++) {
        r.indices[i] = i / qiCount * VERT_COUNT + QUAD_INDICES[i % qi_count];
    }

    glGenVertexArrays(1, &r.vao);
    glBindVertexArray(r.vao);

    glGenBuffers(1, &r.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, r.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vert) * r.vertMax, NULL,
            GL_DYNAMIC_DRAW);

    glGenBuffers(1, &r.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * iCount,
            r.indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, IND_COUNT, GL_FLOAT, GL_FALSE, sizeof(Vert),
            (void*) offsetof(Vert, pos));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, IND_COUNT, GL_FLOAT, GL_FALSE, sizeof(Vert),
            (void*) offsetof(Vert, tex_coord));

    r.vertCount = 0;
    r.verts = (Vert*) malloc(sizeof(Vert) * r.vertMax);

    return r;
}

Rend render_create(size_t count, const char* file) {
    Rend r = create(count);
    r.tex = tex_load(file);
    return r;
}

void render_delete(Rend* r) {
    tex_unload(r->tex);
    if (r->verts)   free(r->verts);
    if (r->indices) free(r->indices);
    glDeleteBuffers(1, &r->ebo);
    glDeleteBuffers(1, &r->vbo);
    glDeleteVertexArrays(1, &r->vao);
}

void render_begin(Rend* r) {
    shader_setTex(shader, r->tex.unit);
}

void flush(Rend* r) {
    if (!r->vertCount) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vert) * r->vertCount,
            r->verts);

    glBindVertexArray(r->vao);
    GLsizei count = r->vertCount / VERT_COUNT * COUNT(QUAD_INDICES);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);

    r->vertCount = 0;
}

void render_end(Rend* r) {
    flush(r);
}

void render_quad(Rend* r, const Quad* q) {
    if (r->vertCount == r->vertMax) {
#ifndef NDEBUG
        fprintf(stderr, "%s\n", "Warning: flushed full vertex cache.");
#endif // !NDEBUG
        flush(r);
    }

    for (size_t i = 0; i < VERT_COUNT; i++) {
        r->verts[r->vertCount++] = q->verts[i];
    }
}
