#pragma once
#undef GLAD_GL_IMPLEMENTATION

#include <cglm/struct.h> // vec2s
#include <glad.h>        // GL*

// Types
typedef struct
{
    GLuint name;
    GLenum unit;
    vec2s  size;
} Tex;

// Function prototypes
Tex  tex_create(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, const void* data);
Tex  tex_load(const char* file);
void tex_unload(Tex tex);
