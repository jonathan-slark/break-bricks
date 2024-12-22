/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
*/

#define GLFW_INCLUDE_NONE
#include <box2d/box2d.h>
#include <cglm/struct.h>
#include <ctype.h>
#include <glad.h>
#include <GLFW/glfw3.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "main.h"
#include "sprite.h"
#include "util.h"

/* Macros
 * Paddle width = 1 m */
#define PIXEL2M(x) ((x) / (float) paddlewidth)
#define PIXEL2EXTENT(x) (PIXEL2M(x / 2.0f))

/* Types */

typedef struct {
    b2BodyId bodyid;
    int isstuck;
    Sprite sprite;
} Ball;

typedef struct {
    b2BodyId bodyid;
    Sprite sprite;
} Paddle;

/* Function prototypes */
static void initball(void);
static void initpaddle(void);
static void setpos(Sprite *s, b2BodyId id);

/* Variables */
static Ball ball;
static Paddle paddle;
static GLuint spritesheet, spriteshader;
static b2WorldId worldid;
static int keypressed[GLFW_KEY_LAST + 1];
static const float timestep = 1.0f / 144.0f;
static const int substepcount = 4;

/* Config uses types from this file */
#include "config.h"

/* Function implementations */

void
initball(void)
{
    Sprite *s = &ball.sprite;
    b2BodyDef ballbd;
    b2Polygon ballbox;
    b2ShapeDef ballsd;

    ball.isstuck = 1;

    memcpy(s->texverts, ballverts, sizeof(ballverts));
    s->size.x = PIXEL2M(ballwidth);
    s->size.y = PIXEL2M(ballheight);
    sprite_init(s);

    ballbd = b2DefaultBodyDef();
    ballbd.type = b2_dynamicBody;
    ballbd.position = (b2Vec2){
	PIXEL2M(scrwidth) / 2.0f,
	PIXEL2M(scrheight) / 2.0f
    };
    ball.bodyid = b2CreateBody(worldid, &ballbd);
    ballbox = b2MakeBox(PIXEL2EXTENT(ballwidth), PIXEL2EXTENT(ballheight));
    ballsd = b2DefaultShapeDef();
    ballsd.density = 1.0f;
    ballsd.friction = 0.3f;
    b2CreatePolygonShape(ball.bodyid, &ballsd, &ballbox);
}

void
initpaddle(void)
{
    Sprite *s = &paddle.sprite;
    b2BodyDef paddlebd;
    b2Polygon paddlebox;
    b2ShapeDef paddlesd;

    memcpy(s->texverts, paddleverts, sizeof(paddleverts));
    s->size.x = PIXEL2M(paddlewidth);
    s->size.y = PIXEL2M(paddleheight);
    sprite_init(s);

    paddlebd = b2DefaultBodyDef();
    paddlebd.position = (b2Vec2){
	PIXEL2M(scrwidth / 2.0f),
	PIXEL2M(paddleheight / 2.0f)
    };
    paddle.bodyid = b2CreateBody(worldid, &paddlebd);
    paddlebox = b2MakeBox(PIXEL2EXTENT(paddlewidth),
	    PIXEL2EXTENT(paddleheight));
    paddlesd = b2DefaultShapeDef();
    b2CreatePolygonShape(paddle.bodyid, &paddlesd, &paddlebox);
}

void
game_load(void)
{
    mat4s proj;
    b2WorldDef worldDef;

    proj = glms_ortho(0.0f, PIXEL2M(scrwidth), 0.0f, PIXEL2M(scrheight), -1.0f,
	    1.0f);
    spriteshader = sprite_shaderload(vertshader, fragshader);
    sprite_shaderuse(spriteshader);
    sprite_shadersetmat4s(spriteshader, projuniform, proj);

    spritesheet = sprite_sheetload(spritefile, 1);
    glActiveTexture(GL_TEXTURE0);
    sprite_sheetuse(spritesheet);
    sprite_shadersetint(spriteshader, texuniform, 0);

    worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -10.0f};
    worldid = b2CreateWorld(&worldDef);

    initball();
    initpaddle();
}

void
game_unload(void)
{
    b2DestroyWorld(worldid);
    sprite_term(&paddle.sprite);
    sprite_term(&ball.sprite);
    sprite_sheetunload(spritesheet);
    sprite_shaderunload(spriteshader);
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
game_input(float dt)
{
    UNUSED(dt);
}

void
game_update(float dt)
{
    UNUSED(dt);

    b2World_Step(worldid, timestep, substepcount);
}

void
setpos(Sprite *s, b2BodyId id)
{
    b2Vec2 pos;

    /* Box2D's origin is in centre of body */
    pos = b2Body_GetPosition(id);
    s->pos.x = pos.x - s->size.x / 2.0f;
    s->pos.y = pos.y - s->size.y / 2.0f;;
}

void
game_render(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    setpos(&ball.sprite, ball.bodyid);
    sprite_draw(spriteshader, &ball.sprite);

    setpos(&paddle.sprite, paddle.bodyid);
    sprite_draw(spriteshader, &paddle.sprite);
}
