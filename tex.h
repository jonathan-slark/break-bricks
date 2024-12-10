/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

GLuint tex_load(const char *name, int isalpha);
void tex_unload(GLuint id);
void tex_bind(GLuint id);
