/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

GLuint shader_load(const char *vertex, const char *fragment);
void shader_unload(GLuint shader);
void shader_use(GLuint shader);
void shader_setint(GLuint shader, const char *name, GLint val);
void shader_setmat4s(GLuint shader, const char *name, mat4s val);
