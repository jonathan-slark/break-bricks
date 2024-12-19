/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#define GLFW_INCLUDE_NONE
#include <cglm/struct.h>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <string.h>

#include "game.h"
#include "sprite.h"
#include "level.h"
#include "main.h"
#include "shader.h"
#include "tex.h"
#include "util.h"

#include "config.h"

/* Types */

typedef struct {
    vec2s vel;
    int isstuck;
    Sprite sprite;
} Ball;

/* Function prototypes */
void initball(void);
void moveball(float dt);
void initpaddle(void);
void movepaddle(float vel);

/* Variables */
static int keypressed[GLFW_KEY_LAST + 1];
static GLuint spritesheet;
static GLuint spriteshader;
static Sprite paddle;
static Ball ball;

/* Function implementations */

void
initball(void)
{
    Sprite *s = &ball.sprite;
    vec2s offset = {{ paddlewidth / 2.0f - ballradius, -ballradius * 2.0f }};;

    ball.vel = ballvelocity;
    ball.isstuck = 1;

    memcpy(s->texverts, ballverts, sizeof(ballverts));
    s->size.x = ballwidth;
    s->size.y = ballheight;
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
    } else if (s->pos.x + ballwidth >= scrwidth) {
	ball.vel.x = -ball.vel.x;
	s->pos.x = scrwidth - ballwidth;
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

    memcpy(s->texverts, paddleverts, sizeof(paddleverts));
    s->size.x = paddlewidth;
    s->size.y = paddleheight;
    s->pos.x = scrwidth / 2.0f - s->size.x / 2.0f;
    s->pos.y = scrheight - s->size.y;

    sprite_init(s);
}

void
game_load(void)
{
    mat4s proj;
    char lvl[] = LVLFOLDER "/00.txt";
    char fmt[] = LVLFOLDER "/%02i.txt";

    proj = glms_ortho(0.0f, scrwidth, scrheight, 0.0f, -1.0f, 1.0f);
    spriteshader = shader_load(vertshader, fragshader);
    shader_use(spriteshader);
    shader_setmat4s(spriteshader, projuniform, proj);

    spritesheet = tex_load(spritefile, 1);
    glActiveTexture(GL_TEXTURE0);
    tex_use(spritesheet);
    shader_setint(spriteshader, texuniform, 0);

    sprintf(lvl, fmt, 1);
    level_load(lvl);
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
    keypressed[key] = 1;
}

void
game_keyup(int key)
{
    keypressed[key] = 0;
}

void
movepaddle(float vel)
{
    paddle.pos.x += vel;
    paddle.pos.x = CLAMP(paddle.pos.x, 0.0f, scrwidth - paddlewidth);

    if (ball.isstuck)
	ball.sprite.pos.x = paddle.pos.x + paddlewidth / 2.0f - ballradius;
}

void
game_movepaddleleft(float dt)
{
    movepaddle(-paddlevelocity * dt);
}

void
game_movepaddleright(float dt)
{
    movepaddle(paddlevelocity * dt);
}

void
game_releaseball(float dt)
{
    UNUSED(dt);

    ball.isstuck = 0;
}

void
game_input(float dt)
{
    size_t i;

    for (i = 0; i < COUNT(keys); i++)
	if (keypressed[keys[i].key])
	    (*keys[i].func)(dt);
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
