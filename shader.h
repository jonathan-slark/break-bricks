/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

GLuint shader_load(const char *vertex, const char *fragment);
void shader_unload(GLuint prog);
void shader_use(GLuint prog);
void shader_setint(GLint loc, GLint val);
void shader_setmat4s(GLint loc, mat4s val);
void shader_setvec3s(GLint loc, vec3s val);
