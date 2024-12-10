/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#include <cglm/cglm.h>

void
sprite_draw(GLuint id, vec2 pos, vec2 size, float rot, vec3 colour)
{
}


void
sprite_drawdef(GLuint id, vec2 pos)
{
    const float sizev[] = { 10.0f, 10.0f };
    vec2 size;
    vec3 colour;

    glm_vec2(sizev, size);
    sprite_draw(id, pos, size, 0.0f, color);
}
