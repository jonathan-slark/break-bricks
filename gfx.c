/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 * TODO: Keep track of current shader to minimise state changes.
 * TODO: Shaders have different uniforms.
 */

#define GL_CONTEXT_FLAG_DEBUG_BIT 0x00000002
#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#define STB_RECT_PACK_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include <cglm/struct.h>
#include <glad.h>
#include <stb/stb_image.h>
#include <stb/stb_rect_pack.h>
#include <stb/stb_truetype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "gfx.h"
#include "main.h"
#include "util.h"

// Types
typedef struct {
    GLuint program;
    GLint  loc_proj;
    GLint  loc_tex;
    GLint  loc_col;
} Shader;

// Function prototypes
#ifndef NDEBUG
static bool   is_member(const unsigned array[], size_t size, unsigned value);
static void GLAPIENTRY gl_debug_output(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length, const GLchar* message, const void* userparam);
#endif // !NDEBUG
static GLint  shader_compile(GLenum type, const GLchar* src);
static Shader shader_load(const char* vertex, const char* fragment);
static void   shader_unload(Shader shader);
static void   shader_use(Shader shader);
static void   shader_set_proj(Shader shader, mat4s proj);
static void   shader_set_tex(Shader shader, GLint tex);
static void   shader_set_colour(Shader shader, vec3s colour);
static Tex    tex_load(const char* file);
static void   tex_unload(Tex tex);
static void   flush(Renderer* r);

// Constants
#ifndef NDEBUG
static const unsigned LOG_IGNORE[] = {
    131185, // Buffer info
};
#endif // !NDEBUG
static const char     SHADER_QUAD_VERT[] = "shader/quad_vert.glsl";
static const char     SHADER_QUAD_FRAG[] = "shader/quad_frag.glsl";
static const char     SHADER_FONT_FRAG[] = "shader/font_frag.glsl";
static const GLchar   UNIFORM_PROJ[]     = "proj";
static const GLchar   UNIFORM_TEX[]      = "tex";
static const GLchar   UNIFORM_COL[]   = "col";
static const GLushort QUAD_INDICES[]     = { 0, 1, 2, 0, 2, 3 };
static const unsigned FONT_QUAD_COUNT    = 200; // Max amount of letter quads

// Variables
Shader shader_quad, shader_font;
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

void GLAPIENTRY gl_debug_output(
    [[maybe_unused]] GLenum source,
    [[maybe_unused]] GLenum type,
    GLuint id,
    [[maybe_unused]] GLenum severity,
    [[maybe_unused]] GLsizei length,
    const GLchar* message,
    [[maybe_unused]] const void* userparam
) {
    if (is_member(LOG_IGNORE, sizeof(LOG_IGNORE), id))
        return;

    fprintf(stderr, "%u: %s\n", id, (const char*) message);
}

#endif // !NDEBUG

void show_log(GLuint object, PFNGLGETSHADERIVPROC proc_param, PFNGLGETSHADERINFOLOGPROC proc_log) {
    GLint len;
    proc_param(object, GL_INFO_LOG_LENGTH, &len);
    if (len) {
	GLchar* log = (GLchar*) malloc(len * sizeof(GLchar));
	proc_log(object, len, &len, log);
	fprintf(stderr, (char*) log);
	free(log);
    }
}

GLint shader_compile(GLenum type, const GLchar* src) {
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint is_compiled = false;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &is_compiled);
    if (!is_compiled) {
	show_log(shader, glGetShaderiv, glGetShaderInfoLog);
	glDeleteShader(shader);
	main_term(EXIT_FAILURE, "Could not texload shader.\n");
    }
    return shader;
}

Shader shader_load(const char* vertex, const char* fragment) {
    GLchar* vsrc = (GLchar*) util_load(vertex, READ_ONLY_TEXT);
    GLchar* fsrc = (GLchar*) util_load(fragment, READ_ONLY_TEXT);
    GLuint v = shader_compile(GL_VERTEX_SHADER, vsrc);
    GLuint f = shader_compile(GL_FRAGMENT_SHADER, fsrc);
    util_unload(vsrc);
    util_unload(fsrc);

    GLuint program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f);
    glLinkProgram(program);
    glDeleteShader(v);
    glDeleteShader(f);

    GLint is_linked = false;
    glGetProgramiv(program, GL_LINK_STATUS, &is_linked);
    if (!is_linked) {
	show_log(program, glGetProgramiv, glGetProgramInfoLog);
	glDeleteProgram(program);
	main_term(EXIT_FAILURE, "Could not link shaders.\n");
    }

    return (Shader) {
	.program  = program,
	.loc_proj = glGetUniformLocation(program, UNIFORM_PROJ),
	.loc_tex  = glGetUniformLocation(program, UNIFORM_TEX),
	.loc_col  = glGetUniformLocation(program, UNIFORM_COL)
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

void shader_set_colour(Shader shader, vec3s col) {
    glUniform3f(shader.loc_col, col.r, col.g, col.b);
}

void gfx_init(void) {
#ifndef NDEBUG
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	glDebugMessageCallbackARB(gl_debug_output, NULL);
	glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, GL_TRUE);
    }
#endif // !NDEBUG

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    shader_quad = shader_load(SHADER_QUAD_VERT, SHADER_QUAD_FRAG);
    shader_font = shader_load(SHADER_QUAD_VERT, SHADER_FONT_FRAG); // Uses same vert shader
    gfx_resize(SCR_WIDTH, SCR_HEIGHT);
}

void gfx_term(void) {
    shader_unload(shader_quad);
}

void gfx_resize(int width, int height) {
    glViewport(0, 0, width, height);

    // Using origin top left to match coords typically used with images
    mat4s proj = glms_ortho(0.0f, width, height, 0.0f, -1.0f, 1.0f);
    shader_use(shader_quad);
    shader_set_proj(shader_quad, proj);
    shader_use(shader_font);
    shader_set_proj(shader_font, proj);
}

Tex tex_load(const char* file) {
    int width, height, chan;
    void* data = stbi_load(file, &width, &height, &chan, 0);
    if (!data) {
	main_term(EXIT_FAILURE, "Could not texload image %s\n.", file);
    }

    GLuint name;
    glGenTextures(1, &name);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, name);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

    return (Tex) {
	.name = name,
	.unit = unit++,
	.size = (vec2s) {{ width, height }}
    };
}

void tex_unload(Tex tex) {
    glDeleteTextures(1, &tex.name);
}

Renderer render_create(size_t count) {
    Renderer r;
    r.vert_max = count * VERT_COUNT;
    size_t qi_count = COUNT(QUAD_INDICES);
    size_t index_count = count * qi_count;

    // Pre-calculate the entire index buffer
    r.indices = (GLushort*) malloc(sizeof(GLushort) * index_count);
    for (size_t i = 0; i < index_count; i++) {
	r.indices[i] = i / qi_count * VERT_COUNT + QUAD_INDICES[i % qi_count];
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
	    (void*) offsetof(Vert, tex_coord));

    r.vert_count = 0;
    r.verts = (Vert*) malloc(sizeof(Vert) * r.vert_max);

    return r;
}

Renderer gfx_render_create(size_t count, const char* file) {
    Renderer r = render_create(count);
    r.tex = tex_load(file);
    return r;
}

void gfx_render_delete(Renderer* r) {
    tex_unload(r->tex);
    if (r->verts)   free(r->verts);
    if (r->indices) free(r->indices);
    glDeleteBuffers(1, &r->ebo);
    glDeleteBuffers(1, &r->vbo);
    glDeleteVertexArrays(1, &r->vao);
}

void gfx_render_begin(Renderer* r) {
    shader_use(shader_quad);
    shader_set_tex(shader_quad, r->tex.unit);
}

void flush(Renderer* r) {
    if (!r->vert_count) {
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, r->vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vert) * r->vert_count,
	    r->verts);

    glBindVertexArray(r->vao);
    GLsizei count = r->vert_count / VERT_COUNT * COUNT(QUAD_INDICES);
    glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, 0);

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
Quad gfx_quad_create(Renderer* r, vec2s pos, vec2s size, vec2s tex_offset) {
    Quad q;

    gfx_quad_set_pos(&q, pos, size);

    float u1 = tex_offset.x            / r->tex.size.s;
    float v1 = tex_offset.y            / r->tex.size.t;
    float u2 = (tex_offset.x + size.s) / r->tex.size.s;
    float v2 = (tex_offset.y + size.y) / r->tex.size.t;

    q.verts[0].tex_coord = (vec2s) {{ u1, v1 }};
    q.verts[1].tex_coord = (vec2s) {{ u2, v1 }};
    q.verts[2].tex_coord = (vec2s) {{ u2, v2 }};
    q.verts[3].tex_coord = (vec2s) {{ u1, v2 }};

    return q;
}

void gfx_quad_set_pos(Quad* q, vec2s pos, vec2s size) {
    float x1 = pos.x;
    float y1 = pos.y;
    float x2 = pos.x + size.s;
    float y2 = pos.y + size.t;

    q->verts[0].pos = (vec2s) {{ x1, y1 }};
    q->verts[1].pos = (vec2s) {{ x2, y1 }};
    q->verts[2].pos = (vec2s) {{ x2, y2 }};
    q->verts[3].pos = (vec2s) {{ x1, y2 }};
}

void gfx_quad_add_vec(Quad* q, vec2s v) {
    for (size_t i = 0; i < VERT_COUNT; i++) {
	q->verts[i].pos = glms_vec2_add(q->verts[i].pos, v);
    }
}

Font gfx_font_create(unsigned height, const char* file) {
    unsigned char* data = (unsigned char*) util_load(file, READ_ONLY_BIN);
    if (stbtt_GetNumberOfFonts(data) < 0) {
	main_term(EXIT_FAILURE, "Loaded font does not contain valid data:\n%s\n", file);
    }

    unsigned char* bitmap = (unsigned char*) malloc(SCR_WIDTH * SCR_HEIGHT * sizeof(unsigned char));

    Font f;
    stbtt_pack_context ctx;
    if (!stbtt_PackBegin(&ctx, bitmap, SCR_WIDTH, SCR_HEIGHT, 0, 1, NULL)) {
	main_term(EXIT_FAILURE, "stbtt_PackBegin failed.\n");
    }
    stbtt_PackFontRange(&ctx, data, 0, height, ASCII_FIRST, ASCII_COUNT, f.chars);
    stbtt_PackEnd(&ctx);

    util_unload((char*) data);

    GLuint name;
    glGenTextures(1, &name);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, name);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_UNSIGNED_BYTE, bitmap);

    free(bitmap);

    f.size   = height;
    f.render = render_create(FONT_QUAD_COUNT);
    f.render.tex = (Tex) {
	.name = name,
	.unit = unit++,
	.size = {{ SCR_WIDTH, SCR_HEIGHT }}
    };

    return f;
}

void gfx_font_delete(Font* f) {
    gfx_render_delete(&f->render);
}

void gfx_font_begin(Font* f) {
    shader_use(shader_font);
    shader_set_tex(shader_font, f->render.tex.unit);
}

void gfx_font_printf(Font* f, vec2s pos, vec3s colour, const char* fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int size = vsnprintf(NULL, 0, fmt, ap);
    char text[size + 1];
    vsnprintf(text, sizeof text, fmt, ap);
    va_end(ap);

    float posx = pos.x;
    shader_set_colour(shader_font, colour);
    for (unsigned i = 0; i < sizeof text - 1; i++) {
	if (text[i] == '\n') {
	    pos.y += f->size;
	    pos.x = posx;
	} else {
	    int j = text[i] - ASCII_FIRST;
	    stbtt_aligned_quad quad;
	    stbtt_GetPackedQuad(&f->chars[0], SCR_WIDTH, SCR_HEIGHT, j, &pos.x, &pos.y, &quad, 0);
	    // Convert to our quad structure
	    float x1 = quad.x0; float y1 = quad.y0;
	    float u1 = quad.s0; float v1 = quad.t0;
	    float x2 = quad.x1; float y2 = quad.y1;
	    float u2 = quad.s1; float v2 = quad.t1;
	    Quad gfx_quad = (Quad) {
		{
		    { {{ x1, y1 }}, {{ u1, v1 }} },
		    { {{ x2, y1 }}, {{ u2, v1 }} },
		    { {{ x2, y2 }}, {{ u2, v2 }} },
		    { {{ x1, y2 }}, {{ u1, v2 }} }
		}
	    };
	    gfx_render_quad(&f->render, &gfx_quad);
	}
    }
}

void gfx_font_end(Font* f) {
    flush(&f->render);
}
