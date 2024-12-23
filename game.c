/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 * TODO:
 * Unlock timestep from framerate.
 * b2World_GetBodyEvents()
 * Add back sprite rotation.
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
#define EXT(x) ((x) / 2.0f)

/* Types */

typedef struct {
    b2BodyId bodyid;
    int isstuck;
    Sprite sprite;
} Ball;

typedef struct {
    b2BodyId bodyid;
    int issolid;
    int isdestroyed;
    Sprite sprite;
} Brick;

typedef struct {
    b2BodyId bodyid;
    Sprite sprite;
} Paddle;

/* Function prototypes */
static void initsprite(Sprite *s, unsigned int width, unsigned int height,
	const unsigned int *verts, size_t size);
static void initbrick(Brick *brick, char id, unsigned int row, unsigned int col);
static unsigned int readbricks(const char *lvl, Brick *bricks);
static void levelload(const char *lvl);
static void initball(void);
static void initpaddle(void);
static void levelunload(void);
static void updatesprite(Sprite *s, b2BodyId id);
static void leveldraw(GLuint shader);

/* Variables */
static Brick *bricks;
static unsigned int brickcount;
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
initsprite(Sprite *s, unsigned int width, unsigned int height,
	const unsigned int *verts, size_t size)
{
    memcpy(s->texverts, verts, size);
    s->size.x = PIXEL2M(width);
    s->size.y = PIXEL2M(height);
    sprite_init(s);
}

void
initbrick(Brick *brick, char id, unsigned int row, unsigned int col)
{
    Sprite *s = &brick->sprite;
    int solid;
    unsigned int i;

    solid = (!isdigit(id));
    if (solid)
        i = id - 'a' + bricktypes;
    else
        i = id - '0';

    brick->issolid = solid;
    brick->isdestroyed = 0;
    memcpy(s->texverts, brickverts[i], sizeof(brickverts[i]));

    s->size.x = brickwidth;
    s->size.y = brickheight;
    s->pos.x  = row * brickwidth;
    s->pos.y  = col * brickheight;

    sprite_init(s);
}

/* Run once with bricks = NULL to get the brick count, a second time with
 * bricks pointing to an array of bricks to be initialised. */
unsigned int
readbricks(const char *lvl, Brick *bricks)
{
    char c;
    unsigned count = 0, row = 0, col = 0;

    while ((c = *lvl++) != '\0') {
        if (c == '#') {
            while ((c = *lvl++) != '\n') {
                /* Comment */
            }
        } else if (c == 'x') {
            /* No brick */
            row++;
        } else if (isdigit(c) || (c >= 'a' && c <= 'f')) {
            if (bricks)
                initbrick(&bricks[count], c, row, col);
            count++;
            row++;
        } else if (c == '\n') {
            /* Ignore blank line */
            if (row > 0)
                col++;
            row = 0;
        } else if (c != ' ' && c != '\t') {
            term(EXIT_FAILURE, "Syntax error in level file.\n");
        }
    }

    return count;
}

void
levelload(const char *name)
{
    char *lvl;

    lvl = load(name);
    brickcount = readbricks(lvl, NULL);
    bricks = (Brick *) malloc(brickcount * sizeof(Brick));
    readbricks(lvl, bricks);
    unload(lvl);
}

void
initball(void)
{
    b2Circle circle = {
	{ PIXEL2M(EXT(ballwidth)), PIXEL2M(EXT(ballwidth)) },
	PIXEL2M(EXT(ballwidth))
    };
    b2BodyDef ballbd;
    b2ShapeDef ballsd;

    ball.isstuck = 1;
    initsprite(&ball.sprite, ballwidth, ballheight, ballverts,
	    sizeof(ballverts));

    ballbd = b2DefaultBodyDef();
    ballbd.type = b2_dynamicBody;
    ballbd.position = (b2Vec2) {
	PIXEL2M(EXT(scrwidth) - EXT(ballwidth)),
	PIXEL2M(EXT(scrheight) - EXT(ballwidth))
    };
    ball.bodyid = b2CreateBody(worldid, &ballbd);

    ballsd = b2DefaultShapeDef();
    ballsd.density = 1.0f;
    ballsd.friction = 0.3f;
    ballsd.restitution = 1.0f;
    b2CreateCircleShape(ball.bodyid, &ballsd, &circle);
}

void
initpaddle(void)
{
    b2Capsule paddlecap;
    b2BodyDef paddlebd;
    b2ShapeDef paddlesd;

    initsprite(&paddle.sprite, paddlewidth, paddleheight, paddleverts,
	    sizeof(paddleverts));

    paddlebd = b2DefaultBodyDef();
    paddlebd.type = b2_kinematicBody;
    paddlebd.position = (b2Vec2) {
	PIXEL2M(EXT(scrwidth) - EXT(paddlewidth)),
	PIXEL2M(EXT(paddleheight))
    };
    paddle.bodyid = b2CreateBody(worldid, &paddlebd);

    paddlecap.center1 = (b2Vec2) {
	PIXEL2M(EXT(paddleheight)),
	0.0f
    };
    paddlecap.center2 = (b2Vec2) {
	PIXEL2M(paddlewidth - EXT(paddleheight)),
	0.0f
    };
    paddlecap.radius = PIXEL2M(EXT(paddleheight));

    paddlesd = b2DefaultShapeDef();
    paddlesd.density = 1.0f;
    paddlesd.friction = 0.3f;
    b2CreateCapsuleShape(paddle.bodyid, &paddlesd, &paddlecap);
}

void
game_load(void)
{
    mat4s proj;
    b2WorldDef worldDef;
    char lvl[] = LVLFOLDER "/00.txt";
    char fmt[] = LVLFOLDER "/%02i.txt";

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
    worldDef.gravity = (b2Vec2) {0.0f, 0.0f};
    worldDef.enableSleep = 0;
    worldid = b2CreateWorld(&worldDef);

    sprintf(lvl, fmt, 1);
    levelload(lvl);
    initball();
    initpaddle();
}

void
levelunload(void)
{
    unsigned int i;

    for (i = 0; i < brickcount; i++)
        sprite_term(&bricks[i].sprite);

    free(bricks);
    brickcount = 0;
}

void
game_unload(void)
{
    b2DestroyWorld(worldid);
    sprite_term(&paddle.sprite);
    sprite_term(&ball.sprite);
    levelunload();
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

    if (ball.isstuck) {
	b2Vec2 force = { 0.0f, -10.0f };
	b2Body_ApplyForceToCenter(ball.bodyid, force, 0);
	ball.isstuck = 0;
    }

    b2World_Step(worldid, timestep, substepcount);
}

void
updatesprite(Sprite *s, b2BodyId id)
{
    b2Vec2 pos;
    b2Rot rot;

    pos = b2Body_GetPosition(id);
    s->pos.x = pos.x;
    s->pos.y = pos.y;

    rot = b2Body_GetRotation(id);
    s->rot = b2Rot_GetAngle(rot);
}

void
leveldraw(GLuint shader)
{
    unsigned int i;

    for (i = 0; i < brickcount; i++)
        if (!bricks[i].isdestroyed)
            sprite_draw(shader, &bricks[i].sprite);
}

void
game_render(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    leveldraw(spriteshader);

    updatesprite(&ball.sprite, ball.bodyid);
    sprite_draw(spriteshader, &ball.sprite);

    updatesprite(&paddle.sprite, paddle.bodyid);
    sprite_draw(spriteshader, &paddle.sprite);
}
