/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 * TODO: Only store QUAD verts once, not per sprite.
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
#include "gfx.h"
#include "util.h"

/* Macros */
#define SCR2NORM(x, extent) (((x) + 0.5f) / (extent))

/* Types */
enum {
    Verts,
    TexVerts
};

/* Function prototypes */
static GLint createshader(GLenum type, const GLchar* src);
static GLuint shaderload(const char* vertex, const char* fragment);
static GLuint loadtex(const char* name, GLint intformat, GLenum imgformat,
		      GLint wraps, GLint wrapt, GLint filtermin, GLint filtermax);
static void screentonormal(const unsigned* vin, unsigned count, unsigned width,
			   unsigned height, float* vout);

/* Constants */
static const char   SHADER_VERT[]   = "shader/sprite_vert.glsl";
static const char   SHADER_FRAG[]   = "shader/sprite_frag.glsl";
static const GLchar UNIFORM_MODEL[] = "model";
static const GLchar UNIFORM_PROJ[]  = "proj";
static const GLchar UNIFORM_TEX[]   = "tex";
static const float  QUAD[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
};

/* Variables */
static GLuint shader = 0;

/* Function declarations */

GLint createshader(GLenum type, const GLchar* src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);

    GLint iscompiled = false;
    glGetShaderiv(s, GL_COMPILE_STATUS, &iscompiled);
    if (!iscompiled) {
	GLint len = 0;
	glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
	if (len) {
	    GLchar* log = (GLchar*)malloc(len * sizeof(GLchar));
	    glGetShaderInfoLog(s, len, &len, &log[0]);
	    fprintf(stderr, (char*)log);
	    free(log);
	    glDeleteShader(s);
	}
	term(EXIT_FAILURE, "Could not loadtex shader.\n");
    }
    return s;
}

GLuint shaderload(const char* vertex, const char* fragment) {
    GLchar* vsrc = (GLchar*) util_load(vertex);
    GLchar* fsrc = (GLchar*) util_load(fragment);
    GLuint v = createshader(GL_VERTEX_SHADER, vsrc);
    GLuint f = createshader(GL_FRAGMENT_SHADER, fsrc);
    util_unload(vsrc);
    util_unload(fsrc);

    GLuint shader = glCreateProgram();
    glAttachShader(shader, v);
    glAttachShader(shader, f);
    glLinkProgram(shader);
    glDeleteShader(v);
    glDeleteShader(f);

    GLint islinked = false;
    glGetProgramiv(shader, GL_LINK_STATUS, &islinked);
    if (!islinked) {
	GLint len = 0;
	glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len) {
	    GLchar* log = (GLchar*)malloc(len * sizeof(GLchar));
	    glGetProgramInfoLog(shader, len, &len, &log[0]);
	    fprintf(stderr, (char*)log);
	    free(log);
	}
	glDeleteProgram(shader);
	term(EXIT_FAILURE, "Could not link shaders.\n");
    }

    return shader;
}

void shaderunload(GLuint shader) {
    glDeleteProgram(shader);
}

void shaderuse(GLuint shader) {
    glUseProgram(shader);
}

void shadersetint(GLuint shader, const char* name, GLint val) {
    GLint loc = glGetUniformLocation(shader, name);
    if (loc == -1)
	fprintf(stderr, "Could not get uniform location.\n");
    glUniform1i(loc, val);
}

void shadersetmat4s(GLuint shader, const char* name, mat4s val) {
    GLint loc = glGetUniformLocation(shader, name);
    if (loc == -1)
	fprintf(stderr, "Could not get uniform location.\n");
    glUniformMatrix4fv(loc, 1, GL_FALSE, val.raw[0]);
}

void gfx_init(void) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Using origin top left to match coords typically used with images */
    mat4s proj = glms_ortho(0.0f, SCR_WIDTH, SCR_HEIGHT, 0.0f, -1.0f, 1.0f);
    shader = shaderload(SHADER_VERT, SHADER_FRAG);
    shaderuse(shader);
    shadersetmat4s(shader, UNIFORM_PROJ, proj);

    glActiveTexture(GL_TEXTURE0);
    shadersetint(shader, UNIFORM_TEX, 0);
}

void gfx_term(void) {
    shaderunload(shader);
}

GLuint loadtex(const char* name, GLint intformat, GLenum imgformat,
	       GLint wraps, GLint wrapt, GLint filtermin, GLint filtermax) {
    int width = 0, height = 0, chan = 0;
    unsigned char* data = stbi_load(name, &width, &height, &chan, 0);
    if (!data)
	term(EXIT_FAILURE, "Could not loadtex image %s\n.", name);

    GLuint id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, intformat, width, height, 0, imgformat,
		 GL_UNSIGNED_BYTE, (const void*)data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wraps);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapt);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtermin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtermax);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return id;
}

GLuint gfx_ss_load(const char* name, int isalpha) {
    if (isalpha)
	return loadtex(name, GL_RGBA, GL_RGBA, GL_REPEAT, GL_REPEAT, GL_LINEAR,
		       GL_LINEAR);
    else
	return loadtex(name, GL_RGB, GL_RGB, GL_REPEAT, GL_REPEAT, GL_LINEAR,
		       GL_LINEAR);
}

void gfx_ss_unload(GLuint id) {
    glDeleteTextures(1, &id);
}

void gfx_ss_use(GLuint id) {
    glBindTexture(GL_TEXTURE_2D, id);
}

/* https://stackoverflow.com/q/40574677 */
void screentonormal(const unsigned* vin, unsigned count, unsigned width,
		    unsigned height, float* vout) {
    for (unsigned i = 0; i < count; i += INDCOUNT) {
	vout[i] = SCR2NORM(vin[i], width);
	vout[i + 1] = SCR2NORM(vin[i + 1], height);
    }
}

void gfx_sprite_init(Sprite* s) {
    float texverts[ARRAYCOUNT];
    screentonormal(s->texverts, ARRAYCOUNT, SCR_WIDTH, SCR_HEIGHT, texverts);

    glGenVertexArrays(1, &s->vao);
    glBindVertexArray(s->vao);
    glGenBuffers(VBOCOUNT, s->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, s->vbo[Verts]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(QUAD), QUAD, GL_STATIC_DRAW);
    glVertexAttribPointer(0, INDCOUNT, GL_FLOAT, GL_FALSE,
			  INDCOUNT * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, s->vbo[TexVerts]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texverts), texverts, GL_STATIC_DRAW);
    glVertexAttribPointer(1, INDCOUNT, GL_FLOAT, GL_FALSE,
			  INDCOUNT * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void gfx_sprite_term(const Sprite* s) {
    glDeleteVertexArrays(1, &s->vao);
    glDeleteBuffers(VBOCOUNT, s->vbo);
}

void gfx_sprite_draw(const Sprite* s) {
    /* Move to position */
    vec3s pos3 = {{s->pos.x, s->pos.y, 0.0f}};
    mat4s model = glms_translate_make(pos3);

    /* Move origin to centre, rotate, move origin back */
    vec3s prerot = {{s->size.x / 2.0f, s->size.y / 2.0f, 0.0f}};
    model = glms_translate(model, prerot);
    vec3s axis = {{0.0f, 0.0f, 1.0f}};
    model = glms_rotate(model, s->rot, axis);
    vec3s postrot = {{-s->size.x / 2.0f, -s->size.y / 2.0f, 0.0f}};
    model = glms_translate(model, postrot);

    /* Scale to size */
    vec3s size3 = {{s->size.x, s->size.y, 1.0f}};
    model = glms_scale(model, size3);

    shadersetmat4s(shader, UNIFORM_MODEL, model);

    glBindVertexArray(s->vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, VERTCOUNT);
    glBindVertexArray(0);
}
