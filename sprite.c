/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 * TODO: Only store quad verts once, not per sprite.
*/

#include <cglm/struct.h>
#include <glad.h>
#include <stdio.h>

#include "config.h"
#include "shader.h"
#include "sprite.h"

/* Function prototypes */
static void screentonormal(const unsigned int *vin, int count, int width,
	int height, float *vout);

/* Variables */
static const float quad[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
};

/* Function declarations */

/* https://stackoverflow.com/q/40574677 */
void
screentonormal(const unsigned int *vin, int count, int width, int height, float *vout)
{
    int i;

    for (i = 0; i < count; i += INDCOUNT) {
	vout[i]   = ((float) vin[i]   + 0.5f) / (float) width;
	vout[i+1] = ((float) vin[i+1] + 0.5f) / (float) height;
    }
}

void
sprite_init(Sprite *s)
{
    float texverts[ARRAYCOUNT];

    screentonormal(s->texverts, ARRAYCOUNT, scrwidth, scrheight, texverts);

    glGenVertexArrays(1, &s->vao);
    glBindVertexArray(s->vao);
    glGenBuffers(VBOCOUNT, s->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, s->vbo[Verts]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, INDCOUNT, GL_FLOAT, GL_FALSE, 
	    INDCOUNT * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, s->vbo[TexVerts]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texverts), texverts, GL_STATIC_DRAW);
    glVertexAttribPointer(1, INDCOUNT, GL_FLOAT, GL_FALSE, 
	    INDCOUNT * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void
sprite_term(const Sprite *s)
{
    glDeleteVertexArrays(1, &s->vao);
    glDeleteBuffers(VBOCOUNT, s->vbo);
}

void
sprite_draw(GLuint shader, const Sprite *s)
{
    mat4s model;
    vec3s pos3 = {{ s->pos.x, s->pos.y, 0.0f }};
    vec3s size3 = {{ s->size.x, s->size.y, 1.0f }};

    model = glms_translate_make(pos3);
    model = glms_scale(model, size3);
    shader_setmat4s(shader, modeluniform, model);

    glBindVertexArray(s->vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, VERTCOUNT);
    glBindVertexArray(0);
}
