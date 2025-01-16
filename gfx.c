/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 * TODO:
 * Scale to resolution.
 * Store uniform locations.
 * Normalise texverts once and using texture width etc, not screen width.
 */

#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#include <assert.h>
#include <ctype.h>
#include <cglm/struct.h>
#include <glad.h>
#include <stb/stb_image.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "gfx.h"
#include "main.h"
#include "util.h"

// Macros
#define NORMALISE(x, extent) (((x) + 0.5f) / (extent))

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
static void   normalise(const unsigned* vin, unsigned count, unsigned width,
	unsigned height, float* vout);

// Constants
#ifndef NDEBUG
static const unsigned LOG_IGNORE[] = {
    131185, // Buffer info
};
#endif // !NDEBUG
static const char   SHADER_VERT[]   = "shader/sprite_vert.glsl";
static const char   SHADER_FRAG[]   = "shader/sprite_frag.glsl";
static const GLchar UNIFORM_PROJ[]  = "proj";
static const GLchar UNIFORM_TEX[]   = "tex";

// Variables
static GLuint program;
static mat4s proj;

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

void gfx_init(void) {
    gfx_resize(SCR_WIDTH, SCR_HEIGHT);

#ifndef NDEBUG
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(gl_debug_output, NULL);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
		GL_TRUE);
    }
#endif // !NDEBUG

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    program = shader_load(SHADER_VERT, SHADER_FRAG);
}

void gfx_term(void) {
    shader_unload(program);
}

void gfx_resize(int width, int height) {
    glViewport(0, 0, width, height);

    // Using origin top left to match coords typically used with images
    proj = glms_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
}

Tex gfx_tex_load(const char* name) {
    Tex tex;
    int chan;
    unsigned char* data = stbi_load(name, &tex.width, &tex.height, &chan, 0);
    if (!data)
	main_term(EXIT_FAILURE, "Could not texload image %s\n.", name);

    glGenTextures(1, &tex.id);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex.width, tex.height, 0,
	    GL_RGBA, GL_UNSIGNED_BYTE, (const void*) data);

    stbi_image_free(data);

    return tex;
}

void gfx_tex_unload(Tex tex) {
    glDeleteTextures(1, &tex.id);
}

Renderer gfx_render_create(size_t cap, Tex tex) {
    assert(!(cap % VERTCOUNT));

    Renderer r;

    glGenVertexArrays(1, &r.vao);
    glBindVertexArray(r.vao);

    glGenBuffers(1, &r.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, r.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vert) * cap, NULL, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, INDCOUNT, GL_FLOAT, GL_FALSE, sizeof(Vert),
	    (void*) offsetof(Vert, pos));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, INDCOUNT, GL_FLOAT, GL_FALSE, sizeof(Vert),
	    (void*) offsetof(Vert, texcoord));

    r.count = 0;
    r.cap = cap;
    r.verts = (Vert*) malloc(sizeof(Vert) * cap);
    r.tex = tex.id;

    return r;
}

void gfx_render_delete(Renderer* r) {
    free(r->verts);
    glDeleteBuffers(1, &r->vbo);
    glDeleteVertexArrays(1, &r->vao);
}

void gfx_render_flush(Renderer* r) {
    if (!r->count) {
        return;
    }

    shader_use(program);
    shader_set_int(program,   UNIFORM_TEX,  r->tex);
    shader_set_mat4s(program, UNIFORM_PROJ, proj);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, r->tex);

    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vert) * r->count, r->verts);

    glBindVertexArray(r->vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, r->count);

    r->count = 0;
}

// https://stackoverflow.com/q/40574677
void normalise(const unsigned* vin, unsigned count, unsigned width,
       unsigned height, float* vout) {
    for (unsigned i = 0; i < count; i += INDCOUNT) {
       vout[i]     = NORMALISE(vin[i],     width);
       vout[i + 1] = NORMALISE(vin[i + 1], height);
    }
}

void gfx_render_push(Renderer* r, Sprite* s) {
    if (r->count == r->cap) {
	gfx_render_flush(r);
    }

    float texverts[ARRAYCOUNT];
    normalise(s->texverts, ARRAYCOUNT, SCR_WIDTH, SCR_HEIGHT, texverts);

    r->verts[r->count++] = (Vert) {
	.pos      = { s->pos.x,             s->pos.y },
	.texcoord = { texverts[0],          texverts[1] }
    };
    r->verts[r->count++] = (Vert) {
	.pos      = { s->pos.x + s->size.u, s->pos.y },
	.texcoord = { texverts[2],          texverts[3] }
    };
    r->verts[r->count++] = (Vert) {
	.pos      = { s->pos.x,             s->pos.y + s->size.v },
	.texcoord = { texverts[4],          texverts[5] }
    };
    r->verts[r->count++] = (Vert) {
	.pos      = { s->pos.x + s->size.u, s->pos.y + s->size.v},
	.texcoord = { texverts[6],          texverts[7] }
    };
}
