/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#define INDCOUNT   2	/* Number of indices per vertex */
#define VERTCOUNT  4	/* Number of vertices per sprite */
#define ARRAYCOUNT (INDCOUNT * VERTCOUNT)
#define VBOCOUNT 2	/* One for Verts, one for TexVerts */

enum { Verts, TexVerts };
typedef struct {
    unsigned int texverts[ARRAYCOUNT];
    GLuint vao, vbo[VBOCOUNT];
    vec2s size, pos;
} Sprite;

void sprite_init(Sprite *s);
void sprite_term(const Sprite *s);
void sprite_draw(GLuint shader, const Sprite *s);
