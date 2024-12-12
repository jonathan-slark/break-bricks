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
GameState state;
int keys[GLFW_KEY_LAST + 1];
GLuint texid;

/* Function implementations */

void
game_load(void)
{
    mat4s proj;

    proj = glms_ortho(0.0f, (float) scrwidth, (float) scrheight, 0.0f, -1.0f,
	    1.0f);
    texid = tex_load(sprite_tex, 1);
    shader_load(shader_vertex, shader_fragment);

    shader_use();
    tex_use(texid);
    shader_setmat4s(shader_projloc, proj);
    //shader_setuint(shader_texloc, texid);

    sprite_init();
}

void
game_unload(void)
{
    sprite_term();
    tex_unload(texid);
    shader_unload();
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
    vec2s pos = {{ 200.0f, 200.0f }};
    vec2s size = {{ 300.0f, 400.0f }};
    float rot = 45.0f;
    vec3s col = {{ 0.0f, 1.0f, 0.0f }};

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    shader_use();
    tex_use(texid);
    sprite_draw(pos, size, rot, col);
}
