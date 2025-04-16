#pragma once

#include <cglm/struct.h> // vec3s

#include "shader.h"

// Function prototypes
#ifndef NDEBUG
void   gfx_screenshot(void);
#endif
void   gfx_init(void);
void   gfx_term(void);
void   gfx_resize(int width, int height);
void   gfx_clear(vec3s col);
Shader gfx_getShader(void);
