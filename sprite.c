/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 * TODO: Only store quad verts once, not per sprite.
*/

#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#include <ctype.h>
#include <cglm/struct.h>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "sprite.h"
#include "util.h"

/* Macros */
#define SCR2NORM(x, extent) (((x) + 0.5f) / (extent))

/* Function prototypes */
static GLint createshader(GLenum type, const GLchar *src);
static GLuint loadtex(const char *name, GLint intformat, GLenum imgformat,
	GLint wraps, GLint wrapt, GLint filtermin, GLint filtermax);
static void screentonormal(const unsigned int *vin, int count, int width,
	int height, float *vout);

/* Function declarations */

GLint
createshader(GLenum type, const GLchar *src)
{
    GLuint s;
    GLint iscompiled, len;
    GLchar *log;

    s = glCreateShader(type);
    /* Requires OpenGL 4.6 */
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    glGetShaderiv(s, GL_COMPILE_STATUS, &iscompiled);
    if (!iscompiled) {
	glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
	if (len) {
	    log = (GLchar *) malloc(len * sizeof(GLchar));
	    glGetShaderInfoLog(s, len, &len, &log[0]);
	    fprintf(stderr, (char *) log);
	    free(log);
	    glDeleteShader(s);
	}
	term(EXIT_FAILURE, "Could not loadtex shader.\n");
    }
    return s;
}

GLuint
sprite_shaderload(const char *vertex, const char *fragment)
{
    GLchar *vsrc, *fsrc;
    GLuint v, f, shader;
    GLint islinked, len;
    GLchar *log;
    
    vsrc = (GLchar *) load(vertex);
    fsrc = (GLchar *) load(fragment);
    v = createshader(GL_VERTEX_SHADER, vsrc);
    f = createshader(GL_FRAGMENT_SHADER, fsrc);
    unload(vsrc);
    unload(fsrc);

    shader = glCreateProgram();
    glAttachShader(shader, v);
    glAttachShader(shader, f);
    glLinkProgram(shader);
    glDeleteShader(v);
    glDeleteShader(f);

    glGetProgramiv(shader, GL_LINK_STATUS, &islinked);
    if (!islinked) {
	glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len) {
	    log = (GLchar *) malloc(len * sizeof(GLchar));
	    glGetProgramInfoLog(shader, len, &len, &log[0]);
	    fprintf(stderr, (char *) log);
	    free(log);
	}
	glDeleteProgram(shader);
	term(EXIT_FAILURE, "Could not link shaders.\n");
    }

    return shader;
}

void
sprite_shaderunload(GLuint shader)
{
    glDeleteProgram(shader);
}

void
sprite_shaderuse(GLuint shader)
{
    glUseProgram(shader);
}

void
sprite_shadersetint(GLuint shader, const char *name, GLint val)
{
    GLint loc;

    if((loc = glGetUniformLocation(shader, name)) == -1)
	fprintf(stderr, "Could not get uniform location.\n");
    glUniform1i(loc, val);
}

void
sprite_shadersetmat4s(GLuint shader, const char *name, mat4s val)
{
    GLint loc;

    if ((loc = glGetUniformLocation(shader, name)) == -1)
	fprintf(stderr, "Could not get uniform location.\n");
    glUniformMatrix4fv(loc, 1, GL_FALSE, val.raw[0]);
}

GLuint
loadtex(const char *name, GLint intformat, GLenum imgformat, GLint wraps,
	GLint wrapt, GLint filtermin, GLint filtermax)
{
    GLuint id;
    int width, height, chan;
    unsigned char *data;

    data = stbi_load(name, &width, &height, &chan, 0);
    if (!data)
	term(EXIT_FAILURE, "Could not loadtex image %s\n.", name);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, intformat, width, height, 0, imgformat,
	    GL_UNSIGNED_BYTE, (const void *) data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wraps);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapt);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtermin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtermax);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return id;
}

GLuint
sprite_sheetload(const char *name, int isalpha)
{
    if (isalpha)
	return loadtex(name, GL_RGBA, GL_RGBA, GL_REPEAT, GL_REPEAT, GL_LINEAR,
		GL_LINEAR);
    else
	return loadtex(name, GL_RGB, GL_RGB, GL_REPEAT, GL_REPEAT, GL_LINEAR,
		GL_LINEAR);
}

void
sprite_sheetunload(GLuint id)
{
    glDeleteTextures(1, &id);
}

void
sprite_sheetuse(GLuint id)
{
    glBindTexture(GL_TEXTURE_2D, id);
}

/* https://stackoverflow.com/q/40574677 */
void
screentonormal(const unsigned int *vin, int count, int width, int height, float *vout)
{
    int i;

    for (i = 0; i < count; i += INDCOUNT) {
	vout[i]   = SCR2NORM(vin[i],   width);
	vout[i+1] = SCR2NORM(vin[i+1], height);
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
    sprite_shadersetmat4s(shader, modeluniform, model);

    glBindVertexArray(s->vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, VERTCOUNT);
    glBindVertexArray(0);
}

