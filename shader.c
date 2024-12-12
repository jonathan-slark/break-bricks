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
static GLint create(GLenum type, const char *code, size_t size);

/* Variables */
static const char readonlybin[] = "rb";
static const GLchar shaderentry[] = "main";

/* Function implementations */

char *
load(const char *filename, size_t *size)
{
    FILE *fp;
    char *code;

    if ((fp = fopen(filename, readonlybin)) == NULL)
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

GLint
create(GLenum type, const char *code, size_t size)
{
    GLuint s;
    GLint iscompiled, len;
    GLchar *log;

    s = glCreateShader(type);
    /* Requires OpenGL 4.6 */
    glShaderBinary(1, &s, GL_SHADER_BINARY_FORMAT_SPIR_V,
	    (const void *) code, size);
    glSpecializeShader(s, shaderentry, 0, NULL, NULL);
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
    size_t vsize, fsize;
    char *vcode, *fcode;
    GLuint v, f, prog;
    GLint islinked, len;
    GLchar *log;
    
    vcode = load(vertex, &vsize);
    fcode = load(fragment, &fsize);
    v = create(GL_VERTEX_SHADER, vcode, vsize);
    f = create(GL_FRAGMENT_SHADER, fcode, fsize);
    unload(&vcode);
    unload(&fcode);

    prog = glCreateProgram();
    glAttachShader(prog, v);
    glAttachShader(prog, f);
    glLinkProgram(prog);
    glDeleteShader(v);
    glDeleteShader(f);

    glGetProgramiv(prog, GL_LINK_STATUS, &islinked);
    if (!islinked) {
	glGetProgramiv(prog, GL_INFO_LOG_LENGTH, &len);
	if (len) {
	    log = (GLchar *) malloc(len * sizeof(GLchar));
	    glGetProgramInfoLog(prog, len, &len, &log[0]);
	    fprintf(stderr, (char *) log);
	    free(log);
	}
	glDeleteProgram(prog);
	term(EXIT_FAILURE, "Could not link shaders.\n");
    }

    return prog;
}

void
shader_unload(GLuint prog)
{
    glDeleteProgram(prog);
}

void
shader_use(GLuint prog)
{
    glUseProgram(prog);
}

void
shader_setint(GLint loc, GLint val)
{
    glUniform1i(loc, val);
}

void
shader_setmat4s(GLint loc, mat4s val)
{
    glUniformMatrix4fv(loc, 1, GL_FALSE, (float *) val.raw);
}

void
shader_setvec3s(GLint loc, vec3s val)
{
    glUniform3fv(loc, 1, val.raw);
}
