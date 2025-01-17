/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 * TODO: Scale to resolution.
 */

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
// https://stackoverflow.com/q/40574677
#define NORMALISE(x, extent) (((x) + 0.5f) / (extent))

// Types
typedef struct {
    GLuint program;
    GLint  loc_proj;
    GLint  loc_tex;
} Shader;

// Function prototypes
#ifndef NDEBUG
static bool   is_member(const unsigned array[], size_t size, unsigned value);
static void GLAPIENTRY gl_debug_output(GLenum source, GLenum type, GLuint id,
	GLenum severity, GLsizei length, const GLchar* message,
	const void* userparam);
#endif // !NDEBUG
static GLint  shader_create(GLenum type, const GLchar* src);
static Shader shader_load(const char* vertex, const char* fragment);
static void   shader_unload(Shader shader);
static void   shader_use(Shader shader);
static void   shader_set_proj(Shader shader, mat4s proj);
static void   shader_set_tex(Shader shader, GLint tex);
static void   flush(Renderer* r);

// Constants
#ifndef NDEBUG
static const unsigned LOG_IGNORE[] = {
    131185, // Buffer info
};
#endif // !NDEBUG
static const char   SHADER_VERT[]    = "shader/sprite_vert.glsl";
static const char   SHADER_FRAG[]    = "shader/sprite_frag.glsl";
static const GLchar UNIFORM_PROJ[]   = "proj";
static const GLchar UNIFORM_TEX[]    = "tex";
static const GLushort quad_indices[] = { 0, 1, 2, 0, 2, 3 };

// Variables
Shader shader;
GLenum unit = 0;

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

Shader shader_load(const char* vertex, const char* fragment) {
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

    return (Shader) {
	.program  = program,
	.loc_proj = glGetUniformLocation(program, UNIFORM_PROJ),
	.loc_tex  = glGetUniformLocation(program, UNIFORM_TEX)
    };
}

void shader_unload(Shader shader) {
    glDeleteProgram(shader.program);
}

void shader_use(Shader shader) {
    glUseProgram(shader.program);
}

void shader_set_proj(Shader shader, mat4s proj) {
    glUniformMatrix4fv(shader.loc_proj, 1, GL_FALSE, proj.raw[0]);
}

void shader_set_tex(Shader shader, GLint tex) {
    glUniform1i(shader.loc_tex, tex);
}

void gfx_init(void) {
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

    shader = shader_load(SHADER_VERT, SHADER_FRAG);
    shader_use(shader);
    gfx_resize(SCR_WIDTH, SCR_HEIGHT);
}

void gfx_term(void) {
    shader_unload(shader);
}

void gfx_resize(int width, int height) {
    glViewport(0, 0, width, height);

    // Using origin top left to match coords typically used with images
    mat4s proj = glms_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
    shader_set_proj(shader, proj);
}

Tex gfx_tex_load(const char* file) {
    int width, height, chan;
    void* data = stbi_load(file, &width, &height, &chan, 0);
    if (!data)
	main_term(EXIT_FAILURE, "Could not texload image %s\n.", file);

    GLuint name;
    glGenTextures(1, &name);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, name);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA,
	    GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    return (Tex) {
	.name   = name,
	.unit   = unit++,
	.size   = (vec2s) {{ width, height }}
    };
}

void gfx_tex_unload(Tex tex) {
    glDeleteTextures(1, &tex.name);
}

Renderer gfx_render_create(size_t count, Tex tex) {
    Renderer r;
    r.vert_max = count * VERT_COUNT;
    size_t index_count = count * COUNT(quad_indices);

    // Pre-calculate the entire index buffer
    r.indices = (GLushort*) malloc(sizeof(GLushort) * index_count);
    for (size_t i = 0; i < index_count; i++) {
	r.indices[i] = i / COUNT(quad_indices) * VERT_COUNT +
	    quad_indices[i % COUNT(quad_indices)];
    }

    glGenVertexArrays(1, &r.vao);
    glBindVertexArray(r.vao);

    glGenBuffers(1, &r.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, r.vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vert) * r.vert_max, NULL,
	    GL_DYNAMIC_DRAW);

    glGenBuffers(1, &r.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, r.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * index_count,
	    r.indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, IND_COUNT, GL_FLOAT, GL_FALSE, sizeof(Vert),
	    (void*) offsetof(Vert, pos));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, IND_COUNT, GL_FLOAT, GL_FALSE, sizeof(Vert),
	    (void*) offsetof(Vert, texcoord));

    r.vert_count = 0;
    r.verts = (Vert*) malloc(sizeof(Vert) * r.vert_max);
    r.tex = tex;

    return r;
}

void gfx_render_delete(Renderer* r) {
    free(r->verts);
    free(r->indices);
    glDeleteBuffers(1, &r->ebo);
    glDeleteBuffers(1, &r->vbo);
    glDeleteVertexArrays(1, &r->vao);
}

void gfx_render_begin(Renderer* r) {
    shader_set_tex(shader, r->tex.unit);
}

void flush(Renderer* r) {
    if (!r->vert_count) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vert) * r->vert_count,
	    r->verts);

    glBindVertexArray(r->vao);
    glDrawElements(GL_TRIANGLES, r->vert_count / VERT_COUNT *
	    COUNT(quad_indices), GL_UNSIGNED_SHORT, 0);

    r->vert_count = 0;
}

void gfx_render_end(Renderer* r) {
    flush(r);
}

void gfx_render_quad(Renderer* r, const Quad* q) {
    if (r->vert_count == r->vert_max) {
#ifndef NDEBUG
	fprintf(stderr, "%s\n", "Warning: flushed full vertex cache.");
#endif // !NDEBUG
	flush(r);
    }

    for (size_t i = 0; i < VERT_COUNT; i++) {
	r->verts[r->vert_count++] = q->verts[i];
    }
}

// Pre-caculate the vertices needed for a textured quad
Quad gfx_quad_create(vec2s pos, vec2s size, vec2s tex_offset, Tex t) {
    float x1 = pos.x;
    float y1 = pos.y;
    float x2 = pos.x + size.s;
    float y2 = pos.y + size.t;

    float u1 = NORMALISE(tex_offset.x,          t.size.s);
    float v1 = NORMALISE(tex_offset.y,          t.size.t);
    float u2 = NORMALISE(tex_offset.x + size.s, t.size.s);
    float v2 = NORMALISE(tex_offset.y + size.y, t.size.t);

    return (Quad) {
	{
	    { {{ x1, y1 }}, {{ u1, v1 }} },
	    { {{ x2, y1 }}, {{ u2, v1 }} },
	    { {{ x2, y2 }}, {{ u2, v2 }} },
	    { {{ x1, y2 }}, {{ u1, v2 }} }
	}
    };
}

vec2s gfx_quad_pos(Quad* q) {
    return q->verts[0].pos;
}

vec2s gfx_quad_size(Quad* q) {
    return (vec2s) {{
        q->verts[0].pos.x - q->verts[2].pos.x,
        q->verts[0].pos.y - q->verts[2].pos.y
    }};
}

void gfx_quad_move(Quad* q, vec2s v) {
    for (size_t i = 0; i < VERT_COUNT; i++) {
	q->verts[i].pos = glms_vec2_add(q->verts[i].pos, v);
    }
}
