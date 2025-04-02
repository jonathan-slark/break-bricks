#pragma once
#undef STB_TRUETYPE_IMPLEMENTATION

#include <cglm/struct.h>      // vec2s, vec3s
#include <stb/stb_truetype.h> // stbtt_packedchar
#include <stdlib.h>           // size_t

#include "rend.h"

// Constants
constexpr size_t ASCII_FIRST = 32;
constexpr size_t ASCII_LAST  = 126;
constexpr size_t ASCII_COUNT = ASCII_LAST + 1 - ASCII_FIRST;

// Types
typedef struct {
    float size;
    Rend  rend;
    stbtt_packedchar chars[ASCII_COUNT];
} Font;

// Function prototypes
Font font_load(float height, const char* file);
void font_unload(Font f);
void font_begin(Font f);
void font_printf(Font* f, vec2s pos, vec3s col, const char* fmt, ...);
void font_end(Font* f);
