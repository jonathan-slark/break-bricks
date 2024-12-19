/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

typedef struct {
    int issolid;
    int isdestroyed;
    Sprite sprite;
} Brick;

void level_load(const char *lvl);
void level_unload(void);
void level_draw(GLuint shader);
void level_breakbricks(Sprite *ball);
int level_iscompleted(void);
