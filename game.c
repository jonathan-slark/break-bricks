/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#define GLFW_INCLUDE_NONE
#include <glad.h>
#include <GLFW/glfw3.h>

#include "config.h"
#include "game.h"
#include "shader.h"
#include "sprite.h"
#include "tex.h"
#include "util.h"

/* Types */
enum GameState { GameActive, GameMenu, GameWin };
typedef enum GameState GameState;

/* Variables */
GameState state;
int keys[GLFW_KEY_LAST + 1];

/* Function implementations */

void
game_load(void)
{
    shader_load(shader_vertex, shader_fragment);
    //tex_load(...);
}

void
game_unload(void)
{
    shader_unload();
    //tex_unload();
}

void
game_keydown(int key)
{
    keys[key] = 1;
}

void
game_keyup(int key)
{
    keys[key] = 0;
}

void
game_input(float deltatime)
{
    UNUSED(deltatime);
}

void
game_update(float deltatime)
{
    UNUSED(deltatime);
}

void
game_render(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}
