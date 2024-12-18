/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#include <cglm/struct.h>
#include <glad.h>
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "shader.h"
#include "util.h"

/* Function declarations */
static GLint create(GLenum type, const GLchar *src);

/* Function implementations */

GLint
create(GLenum type, const GLchar *src)
{
    GLuint s;
    GLint iscompiled, len;
    GLchar *log;

    s = glCreateShader(type);
    /* Requires OpenGL 4.6 */
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    glGetShaderiv(s, GL_COMPILE_STATUS, &iscompiled);
    if (!iscompiled) {
	glGetShaderiv(s, GL_INFO_LOG_LENGTH, &len);
	if (len) {
	    log = (GLchar *) malloc(len * sizeof(GLchar));
	    glGetShaderInfoLog(s, len, &len, &log[0]);
	    fprintf(stderr, (char *) log);
	    free(log);
	    glDeleteShader(s);
	}
	term(EXIT_FAILURE, "Could not load shader.\n");
    }
    return s;
}

GLuint
shader_load(const char *vertex, const char *fragment)
{
    GLchar *vsrc, *fsrc;
    GLuint v, f, shader;
    GLint islinked, len;
    GLchar *log;
    
    vsrc = (GLchar *) load(vertex);
    fsrc = (GLchar *) load(fragment);
    v = create(GL_VERTEX_SHADER, vsrc);
    f = create(GL_FRAGMENT_SHADER, fsrc);
    unload(vsrc);
    unload(fsrc);

    shader = glCreateProgram();
    glAttachShader(shader, v);
    glAttachShader(shader, f);
    glLinkProgram(shader);
    glDeleteShader(v);
    glDeleteShader(f);

    glGetProgramiv(shader, GL_LINK_STATUS, &islinked);
    if (!islinked) {
	glGetProgramiv(shader, GL_INFO_LOG_LENGTH, &len);
	if (len) {
	    log = (GLchar *) malloc(len * sizeof(GLchar));
	    glGetProgramInfoLog(shader, len, &len, &log[0]);
	    fprintf(stderr, (char *) log);
	    free(log);
	}
	glDeleteProgram(shader);
	term(EXIT_FAILURE, "Could not link shaders.\n");
    }

    return shader;
}

void
shader_unload(GLuint shader)
{
    glDeleteProgram(shader);
}

void
shader_use(GLuint shader)
{
    glUseProgram(shader);
}

void
shader_setint(GLuint shader, const char *name, GLint val)
{
    GLint loc;

    if((loc = glGetUniformLocation(shader, name)) == -1)
	fprintf(stderr, "Could not get uniform location.\n");
    glUniform1i(loc, val);
}

void
shader_setmat4s(GLuint shader, const char *name, mat4s val)
{
    GLint loc;

    if ((loc = glGetUniformLocation(shader, name)) == -1)
	fprintf(stderr, "Could not get uniform location.\n");
    glUniformMatrix4fv(loc, 1, GL_FALSE, val.raw[0]);
}
