/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#define STB_IMAGE_IMPLEMENTATION
#include <glad.h>
#include <stb/stb_image.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"

static GLuint load(const char *name, GLint intformat, GLenum imgformat,
	GLint wraps, GLint wrapt, GLint filtermin, GLint filtermax);

GLuint
load(const char *name, GLint intformat, GLenum imgformat, GLint wraps,
	GLint wrapt, GLint filtermin, GLint filtermax)
{
    GLuint id;
    int width, height, chan;
    unsigned char *data;

    data = stbi_load(name, &width, &height, &chan, 0);
    if (!data)
	term(EXIT_FAILURE, "Could not load image %s\n.", name);

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, intformat, width, height, 0, imgformat,
	    GL_UNSIGNED_BYTE, (const void *) data);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wraps);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapt);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filtermin);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filtermax);
    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return id;
}

GLuint
tex_load(const char *name, int isalpha)
{
    if (isalpha)
	return load(name, GL_RGBA, GL_RGBA, GL_REPEAT, GL_REPEAT, GL_LINEAR,
		GL_LINEAR);
    else
	return load(name, GL_RGB, GL_RGB, GL_REPEAT, GL_REPEAT, GL_LINEAR,
		GL_LINEAR);
}

void
tex_unload(GLuint id)
{
    glDeleteTextures(1, &id);
}

void
tex_use(GLuint id)
{
    glBindTexture(GL_TEXTURE_2D, id);
}
