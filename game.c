/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 *
 * Box2D:
 * Paddle width  = 1 m or 128 px or 1/128 m/px
 * Paddle height = 28 px or 28/128 m
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

/* Macros */
#define SCR2BOX2D(x) ((x) / 128.0f)

/* Types */

typedef struct {
    b2BodyId bodyid;
    int isstuck;
    Sprite sprite;
} Ball;

/* Function prototypes */
static void initball(void);

/* Variables */
static Ball ball;
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

    ball.isstuck = 1;

    memcpy(s->texverts, ballverts, sizeof(ballverts));
    s->size.x = SCR2BOX2D(ballwidth);
    s->size.y = SCR2BOX2D(ballwidth);
    sprite_init(s);
}

void
game_load(void)
{
    mat4s proj;
    b2WorldDef worldDef;
    b2BodyDef groundBodyDef, bodyDef;
    b2BodyId groundId;
    b2Polygon groundBox, dynamicBox;
    b2ShapeDef groundShapeDef, shapeDef;

    proj = glms_ortho(0.0f, SCR2BOX2D(scrwidth), 0.0f, SCR2BOX2D(scrheight), -1.0f, 1.0f);
    spriteshader = sprite_shaderload(vertshader, fragshader);
    sprite_shaderuse(spriteshader);
    sprite_shadersetmat4s(spriteshader, projuniform, proj);

    spritesheet = sprite_sheetload(spritefile, 1);
    glActiveTexture(GL_TEXTURE0);
    sprite_sheetuse(spritesheet);
    sprite_shadersetint(spriteshader, texuniform, 0);

    initball();

    worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2){0.0f, -10.0f};
    worldid = b2CreateWorld(&worldDef);

    groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){SCR2BOX2D(scrwidth) / 2.0f, SCR2BOX2D(-1)};
    groundId = b2CreateBody(worldid, &groundBodyDef);
    groundBox = b2MakeBox(SCR2BOX2D(scrwidth), SCR2BOX2D(2));
    groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);
    
    bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){SCR2BOX2D(scrwidth) / 2.0f, SCR2BOX2D(scrheight) / 2.0f};
    ball.bodyid = b2CreateBody(worldid, &bodyDef);
    dynamicBox = b2MakeBox(SCR2BOX2D(ballwidth), SCR2BOX2D(ballwidth));
    shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.friction = 0.3f;
    b2CreatePolygonShape(ball.bodyid, &shapeDef, &dynamicBox);
}

void
game_unload(void)
{
    b2DestroyWorld(worldid);
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
game_render(void)
{
    Sprite *s = &ball.sprite;
    b2Vec2 position;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    position = b2Body_GetPosition(ball.bodyid);
    s->pos.x = position.x;
    s->pos.y = position.y;

    sprite_draw(spriteshader, &ball.sprite);
}
