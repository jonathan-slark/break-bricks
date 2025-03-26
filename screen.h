#pragma once

#include "quad.h"
#include "rend.h"

// Types
typedef struct
{
    Quad quad;
    Rend rend;
} Screen;

// Function prototypes
void screen_init(Screen *s, const char* file);
void screen_render(Screen *s);
