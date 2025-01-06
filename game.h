/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

void game_load(void);
void game_unload(void);
void game_keydown(int key);
void game_keyup(int key);
void game_buttondown(int button);
void game_buttonup(int button);
Mousepos game_input(double frametime, Mousepos mousepos);
void game_update(double frametime);
void game_render(void);
