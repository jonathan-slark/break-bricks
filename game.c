/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#define GLFW_INCLUDE_NONE
#include <cglm/struct.h>
#include <glad.h>
#include <GLFW/glfw3.h>

#include "config.h"
#include "game.h"
#include "sprite.h"
#include "level.h"
#include "main.h"
#include "shader.h"
#include "tex.h"
#include "util.h"

/* Types */
enum GameState { GameActive, GameMenu, GameWin };
typedef enum GameState GameState;

/* Variables */
//static GameState state;
static int keys[GLFW_KEY_LAST + 1];
static GLuint spritesheet;
static GLuint spriteshader;

/* Function implementations */

void
game_load(void)
{
    mat4s proj;

    proj = glms_ortho(0.0f, (float) scrwidth, (float) scrheight, 0.0f, -1.0f,
	    1.0f);
    spriteshader = shader_load(vertshader, fragshader);
    shader_use(spriteshader);
    shader_setmat4s(spriteshader, projuniform, proj);

    spritesheet = tex_load(spritefile, 1);
    glActiveTexture(GL_TEXTURE0);
    tex_use(spritesheet);
    shader_setint(spriteshader, texuniform, 0);

    level_load(lvls[0]);
}

void
game_unload(void)
{
    level_unload();
    tex_unload(spritesheet);
    shader_unload(spriteshader);
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

    level_draw(spriteshader);
}
