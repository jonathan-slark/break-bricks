/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

#define INDCOUNT   2  /* Number of indices per vertex */
#define VERTCOUNT  4  /* Number of vertices per sprite */
#define ARRAYCOUNT (INDCOUNT * VERTCOUNT)

typedef struct {
    /* texverts are in screen coords */
    unsigned texverts[ARRAYCOUNT];
    GLuint vao, vbo;
    vec2s size, pos;
} Sprite;

void   gfx_init(void);
void   gfx_term(void);
void   gfx_resize(int width, int height);

/* Sprite sheet */
GLuint gfx_ss_load(const char* name);
void   gfx_ss_unload(GLuint id);
void   gfx_ss_use(GLuint id);

void   gfx_sprite_init(Sprite* s);
void   gfx_sprite_term(const Sprite* s);
void   gfx_sprite_draw(const Sprite* s);
