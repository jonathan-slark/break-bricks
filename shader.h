/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

void shader_load(const char *vertex, const char *fragment);
void shader_unload();
void shader_use();
void shader_setuint(GLint loc, GLuint val);
void shader_setmat4s(GLint loc, mat4s val);
void shader_setvec3s(GLint loc, vec3s val);
