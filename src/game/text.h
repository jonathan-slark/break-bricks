#pragma once

#include <cglm/struct.h> // vec2s, vec3s

// Types

typedef enum
{
    FontLarge,
    FontMedium,
    FontSizeCount
} FontSize;

typedef struct
{
    FontSize size;
    vec2s    pos;
    vec3s    col;
    char*    fmt;
} Text;

// Function prototypes
void text_rend(Text t, ...);
void text_flush();
