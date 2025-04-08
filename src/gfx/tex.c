#undef  GLAD_GL_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include <cglm/struct.h>   // vec2s
#include <glad.h>          // gl*, GL*
#include <stb/stb_image.h> // stbi_load, stbi_image_free

#include "../main.h"
#include "tex.h"

// Variables
int unit = 0;

// Function definitions

Tex tex_create(GLint internalFormat, GLsizei width, GLsizei height, GLenum format, const void* data)
{
    GLuint name;
    glGenTextures(1, &name);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, name);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    return (Tex) {
        .name = name,
        .unit = unit++,
        .size = (vec2s) {{ (float) width, (float) height }}
    };
}

Tex tex_load(const char* file)
{
    int width, height, chan;
    void* data = stbi_load(file, &width, &height, &chan, 0);
    if (!data) main_term(EXIT_FAILURE, "Could not texload image %s\n.", file);

    Tex tex = tex_create(GL_RGBA32F, width, height, GL_RGBA, data);

    stbi_image_free(data);

    return tex;
}

void tex_unload(Tex tex)
{
    glDeleteTextures(1, &tex.name);
}
