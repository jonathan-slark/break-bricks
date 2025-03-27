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
Screen screen_load(const char* file);
void   screen_unload(Screen s);
void   screen_rend(Screen s);
