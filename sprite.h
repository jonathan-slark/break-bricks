/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#define VERTCOUNT  2
#define INDEXCOUNT 6
#define ARRAYCOUNT (VERTCOUNT * INDEXCOUNT)
typedef struct {
    const float verts[ARRAYCOUNT];
    const int texverts[ARRAYCOUNT];
    GLuint vao;
    int width, height;
} Sprite;

void sprite_init(Sprite *s);
void sprite_term(const Sprite *s);
void sprite_draw(const Sprite *s, vec2s pos, vec2s size);
