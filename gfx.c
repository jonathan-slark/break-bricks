/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 * TODO:
 * OpenGL 2.0 / GLSL 1.10
 * Scale to resolution?
 */

#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#include <ctype.h>
#include <cglm/struct.h>
#include <glad.h>
#include <stb/stb_image.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "gfx.h"
#include "main.h"
#include "util.h"

// Macros
#define NORMALISE(x, extent) (((x) + 0.5f) / (extent))

// Types
enum { Verts, TexVerts };

// Function prototypes
#ifndef NDEBUG
static bool   is_member(const unsigned array[], size_t size, unsigned value);
static void GLAPIENTRY gl_debug_output(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar* message,
	const void* userparam);
#endif // !NDEBUG
static GLint  shader_create(GLenum type, const GLchar* src);
static GLuint shader_load(const char* vertex, const char* fragment);
static void   shader_unload(GLuint program);
static void   shader_use(GLuint program);
static void   shader_set_int(GLuint program, const char* name, GLint val);
static void   shader_set_mat4s(GLuint program, const char* name, mat4s val);
static GLuint buffer_load(GLenum target, GLsizei size, const void* data);
static void   buffer_unload(GLuint vbo);
static void   normalise(const unsigned* vin, unsigned count, unsigned width,
	unsigned height, float* vout);
static vec3s  make_vec3s(vec2s xy, float z);

// Constants
#ifndef NDEBUG
static const unsigned LOG_IGNORE[] = {
    131185, // Buffer info
};
#endif // !NDEBUG
static const char   SHADER_VERT[]   = "shader/sprite_vert.glsl";
static const char   SHADER_FRAG[]   = "shader/sprite_frag.glsl";
static const GLchar UNIFORM_MODEL[] = "model";
static const GLchar UNIFORM_PROJ[]  = "proj";
static const GLchar UNIFORM_TEX[]   = "tex";
static const float  QUAD[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f,
};

// Variables
static GLuint program, quad_vbo;

// Function declarations

#ifndef NDEBUG

bool is_member(const unsigned array[], size_t size, unsigned value) {
    for (size_t i = 0; i < size; i++) {
        if (array[i] == value) {
            return true;
	}
    }

    return false;
}

void GLAPIENTRY gl_debug_output([[maybe_unused]] GLenum source,
	[[maybe_unused]] GLenum type, GLuint id,
	[[maybe_unused]] GLenum severity, [[maybe_unused]] GLsizei length,
	const GLchar* message, [[maybe_unused]] const void* userparam) {
    if (is_member(LOG_IGNORE, sizeof(LOG_IGNORE), id))
        return;

    fprintf(stderr, "%u: %s\n", id, (const char*) message);
}

#endif // !NDEBUG

void show_log(GLuint object, PFNGLGETSHADERIVPROC proc_param,
	PFNGLGETSHADERINFOLOGPROC proc_log) {
    GLint len;
    proc_param(object, GL_INFO_LOG_LENGTH, &len);
    if (len) {
	GLchar* log = (GLchar*) malloc(len * sizeof(GLchar));
	proc_log(object, len, &len, log);
	fprintf(stderr, (char*) log);
	free(log);
    }
}

GLint shader_create(GLenum type, const GLchar* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint iscompiled = false;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &iscompiled);
    if (!iscompiled) {
	show_log(shader, glGetShaderiv, glGetShaderInfoLog);
	glDeleteShader(shader);
	main_term(EXIT_FAILURE, "Could not texload shader.\n");
    }
    return shader;
}

GLuint shader_load(const char* vertex, const char* fragment) {
    GLchar* vsrc = (GLchar*) util_load(vertex);
    GLchar* fsrc = (GLchar*) util_load(fragment);
    GLuint v = shader_create(GL_VERTEX_SHADER, vsrc);
    GLuint f = shader_create(GL_FRAGMENT_SHADER, fsrc);
    util_unload(vsrc);
    util_unload(fsrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f);
    glLinkProgram(program);
    glDeleteShader(v);
    glDeleteShader(f);

    GLint islinked = false;
    glGetProgramiv(program, GL_LINK_STATUS, &islinked);
    if (!islinked) {
	show_log(program, glGetProgramiv, glGetProgramInfoLog);
	glDeleteProgram(program);
	main_term(EXIT_FAILURE, "Could not link shaders.\n");
    }

    return program;
}

void shader_unload(GLuint program) {
    glDeleteProgram(program);
}

void shader_use(GLuint program) {
    glUseProgram(program);
}

void shader_set_int(GLuint program, const char* name, GLint val) {
    GLint loc = glGetUniformLocation(program, name);
    if (loc == -1)
	fprintf(stderr, "Could not get uniform location.\n");
    glUniform1i(loc, val);
}

void shader_set_mat4s(GLuint program, const char* name, mat4s val) {
    GLint loc = glGetUniformLocation(program, name);
    if (loc == -1)
	fprintf(stderr, "Could not get uniform location.\n");
    glUniformMatrix4fv(loc, 1, GL_FALSE, val.raw[0]);
}

GLuint buffer_load(GLenum target, GLsizei size, const void* data) {
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(target, vbo);
    glBufferData(target, size, data, GL_STATIC_DRAW);
    return vbo;
}

void buffer_unload(GLuint vbo) {
    glDeleteBuffers(1, &vbo);
}

void gfx_init(void) {
    gfx_resize(SCR_WIDTH, SCR_HEIGHT);

#ifndef NDEBUG
    if (GLAD_GL_ARB_debug_output) {
        GLint flags = 0;
        glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
        if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
            glDebugMessageCallbackARB(gl_debug_output, NULL);
            glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE,
		    0, NULL, GL_TRUE);
        }
    }
#endif // !NDEBUG

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Using origin top left to match coords typically used with images
    mat4s proj = glms_ortho(0.0f, SCR_WIDTH, SCR_HEIGHT, 0.0f, -1.0f, 1.0f);
    program = shader_load(SHADER_VERT, SHADER_FRAG);
    shader_use(program);
    shader_set_mat4s(program, UNIFORM_PROJ, proj);

    quad_vbo = buffer_load(GL_ARRAY_BUFFER, sizeof(QUAD), QUAD);
}

void gfx_term(void) {
    shader_unload(program);
    buffer_unload(quad_vbo);
}

void gfx_resize(int width, int height) {
    glViewport(0, 0, width, height);
}

GLuint gfx_ss_load(const char* name) {
    int width, height, chan;
    unsigned char* data = stbi_load(name, &width, &height, &chan, 0);
    if (!data)
	main_term(EXIT_FAILURE, "Could not texload image %s\n.", name);

    GLuint id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
		 GL_UNSIGNED_BYTE, (const void*)data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return id;
}

void gfx_ss_unload(GLuint id) {
    glDeleteTextures(1, &id);
}

void gfx_ss_use(GLuint id) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);
    shader_set_int(program, UNIFORM_TEX, 0);
}

// https://stackoverflow.com/q/40574677
void normalise(const unsigned* vin, unsigned count, unsigned width,
	unsigned height, float* vout) {
    for (unsigned i = 0; i < count; i += INDCOUNT) {
	vout[i]     = NORMALISE(vin[i], width);
	vout[i + 1] = NORMALISE(vin[i + 1], height);
    }
}

void gfx_sprite_init(Sprite* s) {
    float texverts[ARRAYCOUNT];
    normalise(s->texverts, ARRAYCOUNT, SCR_WIDTH, SCR_HEIGHT, texverts);

    glGenVertexArrays(1, &s->vao);
    s->vbo = buffer_load(GL_ARRAY_BUFFER, sizeof(texverts), texverts);

    glBindVertexArray(s->vao);

    glBindBuffer(GL_ARRAY_BUFFER, quad_vbo);
    glVertexAttribPointer(0, INDCOUNT, GL_FLOAT, GL_FALSE,
	    INDCOUNT * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, s->vbo);
    glVertexAttribPointer(1, INDCOUNT, GL_FLOAT, GL_FALSE,
	    INDCOUNT * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(1);
}

void gfx_sprite_term(const Sprite* s) {
    glDeleteVertexArrays(1, &s->vao);
    glDeleteBuffers(1, &s->vbo);
}

vec3s make_vec3s(vec2s xy, float z) {
    return (vec3s) {{ xy.x, xy.y, z }};
}

void gfx_sprite_draw(const Sprite* s) {
    // Move to position
    mat4s model = glms_translate_make(make_vec3s(s->pos, 0.0f));
    // Scale to size
    model = glms_scale(model, make_vec3s(s->size, 1.0f));
    shader_set_mat4s(program, UNIFORM_MODEL, model);

    glBindVertexArray(s->vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, VERTCOUNT);
}
