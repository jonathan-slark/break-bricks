/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#define VERTCOUNT  2
#define INDCOUNT   6
#define ARRAYCOUNT (VERTCOUNT * INDCOUNT)
#define VBOCOUNT   2

enum { Verts, TexVerts };
typedef struct {
    const float verts[ARRAYCOUNT];
    const int texverts[ARRAYCOUNT];
    GLuint vao, vbo[VBOCOUNT];
    int width, height, xpos, ypos;
} Sprite;

void sprite_init(Sprite *s);
void sprite_term(const Sprite *s);
void sprite_draw(GLuint shader, const Sprite *s);
