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
#include "shader.h"
#include "sprite.h"
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
static Sprite brick = {
    .verts = {
        0.0f, 1.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f
    },
    .texverts = {
        0,  9,
        31, 0,
        0,  0,

        0,  9,
        31, 9,
        31, 0
    },
    .width = 32,
    .height = 10,
    .xpos = 0,
    .ypos = 0
};

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
    sprite_init(&brick);

    spritesheet = tex_load(spritefile, 1);
    glActiveTexture(GL_TEXTURE0);
    tex_use(spritesheet);
    shader_setint(spriteshader, texuniform, 0);
}

void
game_unload(void)
{
    shader_unload(spriteshader);
    sprite_term(&brick);
    tex_unload(spritesheet);
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

    sprite_draw(spriteshader, &brick);
}
