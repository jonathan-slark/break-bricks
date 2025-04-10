#pragma once

#include <cglm/struct.h> // vec3s

#include "shader.h"

// Function prototypes
void   gfx_init(void);
void   gfx_term(void);
void   gfx_resize(int width, int height);
void   gfx_clear(vec3s col);
Shader gfx_getShader(void);
