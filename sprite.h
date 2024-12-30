/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

#define INDCOUNT 2  /* Number of indices per vertex */
#define VERTCOUNT 4 /* Number of vertices per sprite */
#define ARRAYCOUNT (INDCOUNT * VERTCOUNT)
#define VBOCOUNT 2 /* One for Verts, one for TexVerts */

enum {
    Verts,
    TexVerts
};
typedef struct {
    /* texverts are in screen coords */
    unsigned texverts[ARRAYCOUNT];
    GLuint vao, vbo[VBOCOUNT];
    vec2s size, pos;
    float rot;
} Sprite;

static const GLchar modeluniform[] = "model";
static const float quad[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.0f, 1.0f,
    1.0f, 1.0f
};

GLuint sprite_shaderload(const char *vertex, const char *fragment);
void sprite_shaderunload(GLuint shader);
void sprite_shaderuse(GLuint shader);
void sprite_shadersetint(GLuint shader, const char *name, GLint val);
void sprite_shadersetmat4s(GLuint shader, const char *name, mat4s val);

GLuint sprite_load(const char *name, int isalpha);
void sprite_unload(GLuint id);
void sprite_use(GLuint id);

void sprite_init(Sprite *s);
void sprite_term(const Sprite *s);
void sprite_draw(GLuint shader, const Sprite *s);
