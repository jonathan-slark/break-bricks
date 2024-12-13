/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#include <cglm/struct.h>
#include <glad.h>

#include "config.h"
#include "shader.h"
#include "sprite.h"
#include "util.h"

/* Types */
typedef struct {
    const float verts[24];
    GLuint vao;
    GLuint shader;
} Sprite;

/* Variables */
static Sprite sprite = {
    .verts = {
	/* pos      tex */
	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 0.0f, 0.0f,

	0.0f, 1.0f, 0.0f, 1.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	1.0f, 0.0f, 1.0f, 0.0f
    }
};
static const GLint vertcount = 4;
static const GLint indexcount = 6;

/* Function declarations */

void
sprite_init(mat4s proj)
{
    GLuint vbo;

    glGenVertexArrays(1, &sprite.vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sprite.verts), sprite.verts,
	    GL_STATIC_DRAW);

    glBindVertexArray(sprite.vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, vertcount, GL_FLOAT, GL_FALSE, 
	    vertcount * sizeof(float), (void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    sprite.shader = shader_load(shader_vertex, shader_fragment);
    shader_use(sprite.shader);
    shader_setmat4s(shader_projloc, proj);
    shader_setint(shader_texloc, 0);
}

void
sprite_term(void)
{
    glDeleteVertexArrays(1, &sprite.vao);
    shader_unload(sprite.shader);
}

void
sprite_draw(vec2s pos, vec2s size)
{
    mat4s model;
    vec3s pos3 = {{ pos.x, pos.y, 0.0f }};
    vec3s size3 = {{ size.x, size.y, 1.0f }};

    model = glms_translate_make(pos3);
    model = glms_scale(model, size3);

    shader_use(sprite.shader);
    shader_setmat4s(shader_modelloc, model);

    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(sprite.vao);
    glDrawArrays(GL_TRIANGLES, 0, indexcount);
    glBindVertexArray(0);
}
