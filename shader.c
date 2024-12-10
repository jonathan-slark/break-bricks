/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#include "glad.h"
#include <stdio.h>
#include <stdlib.h>

#include "main.h"
#include "shader.h"

/* Function declarations */
static char *load(const char *filename, size_t *size);
static void unload(char **code);
static GLuint create(GLenum type, const char *code, size_t size);

/* Variables */
static const char readonlybinary[] = "rb";
static const char shaderentry[] = "main";
static GLuint program;

/* Function implementations */

char *
load(const char *filename, size_t *size)
{
    FILE *fp;
    char *code;

    if ((fp = fopen(filename, readonlybinary)) == NULL)
        term(EXIT_FAILURE, "Could not open file %s.\n", filename);

    if (fseek(fp, 0L, SEEK_END) != 0)
        term(EXIT_FAILURE, "Error on seeking file %s.\n", filename);
    *size = ftell(fp);
    rewind(fp);
    code = (char *) malloc(*size * sizeof(char));
    if (fread(code, sizeof(char), *size, fp) < *size)
        term(EXIT_FAILURE, "Error reading file %s.\n", filename);

    if (fclose(fp) == EOF)
        term(EXIT_FAILURE, "Error on closing file %s.\n", filename);

    return code;
}

void
unload(char **code)
{
    free(*code);
}

GLuint
create(GLenum type, const char *code, size_t size)
{
    GLuint s;
    GLint iscompiled, len;
    GLchar *log;

    s = glCreateShader(type);
    /* Requires OpenGL 4.6 */
    glShaderBinary(1, &s, GL_SHADER_BINARY_FORMAT_SPIR_V,
	    (const void *) code, size);
    glSpecializeShader(s, (const GLchar*) shaderentry, 0, NULL,
	    NULL);
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
	return 0;
    }

    return s;
}

int
shader_load(const char *vertex, const char *fragment)
{
    size_t vsize, fsize;
    char *vcode, *fcode;
    GLuint v, f;
    GLint islinked, len;
    GLchar *log;
    
    vcode = load(vertex, &vsize);
    fcode = load(fragment, &fsize);
    v = create(GL_VERTEX_SHADER, vcode, vsize);
    f = create(GL_FRAGMENT_SHADER, fcode, fsize);
    unload(&vcode);
    unload(&fcode);

    if (!v || !f)
	return 0;

    program = glCreateProgram();
    glAttachShader(program, v);
    glAttachShader(program, f);
    glLinkProgram(program);
    glDeleteShader(v);
    glDeleteShader(f);

    glGetProgramiv(program, GL_LINK_STATUS, (int *) &islinked);
    if (!islinked) {
	glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
	if (len) {
	    log = (GLchar *) malloc(len * sizeof(GLchar));
	    glGetProgramInfoLog(program, len, &len, &log[0]);
	    fprintf(stderr, (char *) log);
	    free(log);
	}
	glDeleteProgram(program);
	return 0;
    }

    return 1;
}

void
shader_unload(void)
{
    glDeleteProgram(program);
}
