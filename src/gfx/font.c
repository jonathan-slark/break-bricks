#define STB_RECT_PACK_IMPLEMENTATION
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC

#include <cglm/struct.h>       // vec2s, vec3s
#include <glad.h>              // GL*
#include <stb/stb_rect_pack.h> // Used by stb_truetype.h
#include <stb/stb_truetype.h>  // stbtt_*
#include <stdarg.h>            // va_list, va_start, va_end
#include <stdlib.h>            // malloc, free
#include <stdio.h>             // vsnprintf

#include "../main.h"
#include "../util.h"
#include "font.h"
#include "gfx.h"
#include "sprite.h"
#include "rend.h"
#include "shader.h"
#include "tex.h"

// Constants
static const unsigned FONT_QUAD_COUNT = 200; // Max amount of letter sprites

// Function definitions

Font font_load(float height, const char* file)
{
    unsigned char* data = (unsigned char*) util_load(file, READ_ONLY_BIN);
    if (!data) main_term(EXIT_FAILURE, "Unable to load font: \n%s\n", file);
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

    f.size     = height;
    f.rend     = rend_create(FONT_QUAD_COUNT);
    f.rend.tex = tex_create(GL_R8, SCR_WIDTH, SCR_HEIGHT, GL_RED, (const void*) bitmap);

    free(bitmap);

    return f;
}

void font_unload(Font f)
{
    rend_unload(f.rend);
}

void font_begin(Font f, vec3s col)
{
    Shader s = gfx_getShader();
    shader_setTex(s, f.rend.tex.unit);
    shader_setIsFont(s, true);
    shader_setCol(s, col);
}

void font_printf(Font* f, vec2s pos, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    font_vprintf(f, pos, fmt, args);
    va_end(args);
}

void font_vprintf(Font* f, vec2s pos, const char* fmt, va_list args)
{
    va_list ap;
    va_copy(ap, args);
    int size = vsnprintf(NULL, 0, fmt, ap);
    va_end(ap);

    char text[size + 1];
    vsnprintf(text, sizeof text, fmt, args);

    float posX = pos.x;
    for (unsigned i = 0; i < sizeof text - 1; i++) {
        if (text[i] == '\n') {
            pos.y += f->size;
            pos.x = posX;
        } else {
            int j = text[i] - ASCII_FIRST;
            stbtt_aligned_quad quad;
            stbtt_GetPackedQuad(&f->chars[0], SCR_WIDTH, SCR_HEIGHT, j, &pos.x, &pos.y, &quad, 0);

            // Convert to our sprite
            float x1 = quad.x0; float y1 = quad.y0;
            float u1 = quad.s0; float v1 = quad.t0;
            float x2 = quad.x1; float y2 = quad.y1;
            float u2 = quad.s1; float v2 = quad.t1;
	    // rend_sprite doesn't use .size
            Sprite sprite = {
                .verts = {
                    { {{ x1, y1 }}, {{ u1, v1 }} },
                    { {{ x2, y1 }}, {{ u2, v1 }} },
                    { {{ x2, y2 }}, {{ u2, v2 }} },
                    { {{ x1, y2 }}, {{ u1, v2 }} }
                }
            };
            rend_sprite(&f->rend, sprite);
        }
    }
}

void font_end(Font* f)
{
    rend_end(&f->rend);
}
