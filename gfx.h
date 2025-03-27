#pragma once

#include "shader.h"

// Function prototypes
void   gfx_init(void);
void   gfx_term(void);
void   gfx_resize(int width, int height);
Shader gfx_getShader(void);
