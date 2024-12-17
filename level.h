/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

typedef struct {
    unsigned int row, col;
    vec3s colour;
    int isdestroyed;
} Brick;

void level_load(const char *lvl);
void level_unload(void);
void level_draw(GLuint shader);
int level_iscompleted(void);
