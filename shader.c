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

/* Function declarations */
static GLint create(GLenum type, const GLchar *src);
static GLchar *load(const GLchar *filename);
static void unload(GLchar **src);

/* Variables */
static const GLchar readonly[] = "r";

/* Function implementations */

GLchar *
load(const GLchar *filename)
{
    FILE *fp;
    GLchar *src;
    size_t size;

    if ((fp = fopen(filename, readonly)) == NULL)
        term(EXIT_FAILURE, "Could not open file %s.\n", filename);

    if (fseek(fp, 0L, SEEK_END) != 0)
        term(EXIT_FAILURE, "Error on seeking file %s.\n", filename);
    size = ftell(fp);
    rewind(fp);
    src = (GLchar *) malloc(size * sizeof(char));
    if (fread(src, sizeof(GLchar), size, fp) < size)
        term(EXIT_FAILURE, "Error reading file %s.\n", filename);

    if (fclose(fp) == EOF)
        term(EXIT_FAILURE, "Error on closing file %s.\n", filename);

    return src;
}

void
unload(GLchar **src)
{
    free(*src);
}

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
    
    vsrc = load(vertex);
    fsrc = load(fragment);
    v = create(GL_VERTEX_SHADER, vsrc);
    f = create(GL_FRAGMENT_SHADER, fsrc);
    unload(&vsrc);
    unload(&fsrc);

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
    glUniformMatrix4fv(loc, 1, GL_FALSE, (float *) val.raw);
}

void
shader_setvec3s(GLuint shader, const char *name, vec3s val)
{
    GLint loc;

    if ((loc = glGetUniformLocation(shader, name)) == -1)
	fprintf(stderr, "Could not get uniform location.\n");
    glUniform3fv(loc, 1, val.raw);
}
