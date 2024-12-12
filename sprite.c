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

static const float verts[] = {
    /* pos      tex */
    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 1.0f,
    1.0f, 0.0f, 1.0f, 0.0f
};
static const GLint vertcount = 4;
static const GLint indexcount = 6;
static GLuint vao;

void
sprite_init(void)
{
    GLuint vbo;

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, vertcount, GL_FLOAT, GL_FALSE, vertcount * sizeof(float), (void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void
sprite_term(void)
{
    glDeleteVertexArrays(1, &vao);
}

void
sprite_draw(vec2s pos, vec2s size, float rot, vec3s col)
{
    mat4s model;
    vec3s pos3 = {{ pos.x, pos.y, 0.0f }};
    vec3s size3 = {{ size.x, size.y, 1.0f }};
    vec3s origincentre = {{ 0.5f * size.x, 0.5f * size.y, 0.0f }};
    vec3s originreset = {{ -0.5f * size.x, -0.5f * size.y, 0.0f }};
    vec3s axis = {{ 0.0f, 0.0f, 1.0f }};

    /* Translate */
    model = glms_translate_make(pos3);

    /* Rotate */
    model = glms_translate(model, origincentre);
    model = glms_rotate(model, glm_rad(rot), axis);
    model = glms_translate(model, originreset);

    /* Scale */
    model = glms_scale(model, size3);

    shader_setmat4s(shader_modelloc, model);
    shader_setvec3s(shader_colloc, col);

    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, indexcount);
    glBindVertexArray(0);
}
