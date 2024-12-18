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

typedef struct {
    vec2s vel;
    int isstuck;
    Sprite sprite;
} Ball;

/* Function prototypes */
void initball(void);
void moveball(float dt);
void initpaddle(void);

/* Variables */
//static GameState state;
static int keys[GLFW_KEY_LAST + 1];
static GLuint spritesheet;
static GLuint spriteshader;
static Sprite paddle;
static Ball ball;

/* Function implementations */

void
initball(void)
{
    Sprite *s = &ball.sprite;
    vec2s offset = {{
	(float) paddlewidth / 2.0f - ballradius,
	-ballradius * 2.0f
    }};;

    ball.vel = ballvelocity;
    ball.isstuck = 1;

    /* Top left */
    s->texverts[0] = bricktypes * brickwidth + paddlewidth;
    s->texverts[1] = 0;
    /* Top right */
    s->texverts[2] = bricktypes * brickwidth + paddlewidth + ballwidth - 1;
    s->texverts[3] = 0;
    /* Bottom left */
    s->texverts[4] = bricktypes * brickwidth + paddlewidth;
    s->texverts[5] = ballheight - 1;
    /* Bottom right */
    s->texverts[6] = bricktypes * brickwidth + paddlewidth + ballwidth - 1;
    s->texverts[7] = ballheight - 1;

    s->size.x = (float) ballwidth;
    s->size.y = (float) ballheight;
    s->pos = glms_vec2_add(paddle.pos, offset);

    sprite_init(s);
}

void
moveball(float dt)
{
    Sprite *s = &ball.sprite;

    if (ball.isstuck)
	return;

    s->pos = glms_vec2_add(s->pos, glms_vec2_scale(ball.vel, dt));

    if (s->pos.x <= 0.0f) {
	ball.vel.x = -ball.vel.x;
	s->pos.x = 0.0f;
    } else if ((int) s->pos.x + ballwidth >= scrwidth) {
	ball.vel.x = -ball.vel.x;
	s->pos.x = (float) (scrwidth - ballwidth);
    }

    if (s->pos.y <= 0.0f) {
	ball.vel.y = -ball.vel.y;
	s->pos.y = 0.0f;
    }
}

void
initpaddle(void)
{
    Sprite *s = &paddle;

    /* Top left */
    s->texverts[0] = bricktypes * brickwidth;
    s->texverts[1] = 0;
    /* Top right */
    s->texverts[2] = bricktypes * brickwidth + paddlewidth - 1;
    s->texverts[3] = 0;
    /* Bottom left */
    s->texverts[4] = bricktypes * brickwidth;
    s->texverts[5] = paddleheight - 1;
    /* Bottom right */
    s->texverts[6] = bricktypes * brickwidth + paddlewidth - 1;
    s->texverts[7] = paddleheight - 1;

    s->size.x = (float) paddlewidth;
    s->size.y = (float) paddleheight;
    s->pos.x = ((float) scrwidth) / 2.0f - s->size.x / 2.0f;
    s->pos.y = ((float) scrheight) - s->size.y;

    sprite_init(s);
}

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

    level_load(lvlfolder);
    initpaddle();
    initball();
}

void
game_unload(void)
{
    sprite_term(&ball.sprite);
    sprite_term(&paddle);
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
game_input(float dt)
{
    float vel = paddlevelocity * dt;

    if (keys[GLFW_KEY_A]) {
	paddle.pos.x -= vel;
	paddle.pos.x = MAX(0.0f, paddle.pos.x);
    }

    if (keys[GLFW_KEY_D]) {
	paddle.pos.x += vel;
	paddle.pos.x = MIN((float) (scrwidth - paddlewidth), paddle.pos.x);
    }

    if (ball.isstuck)
	ball.sprite.pos.x = paddle.pos.x + (float) paddlewidth / 2.0f -
	    ballradius;

    if (keys[GLFW_KEY_SPACE])
	ball.isstuck = 0;
}

void
game_update(float dt)
{
    moveball(dt);
}

void
game_render(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    level_draw(spriteshader);
    sprite_draw(spriteshader, &paddle);
    sprite_draw(spriteshader, &ball.sprite);
}
