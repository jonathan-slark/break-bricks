/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

typedef struct {
    int key;
    void (*func)(float dt);
} Key;

void game_load(void);
void game_unload(void);
void game_keydown(int key);
void game_keyup(int key);
void game_movepaddleleft(float dt);
void game_movepaddleright(float dt);
void game_releaseball(float dt);
void game_input(float dt);
void game_update(float dt);
void game_render(void);
