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
#define EXT(x) ((x) / 2.0f)

/* Types */

typedef struct {
    int key;
    void (*func)(float frametime);
} Key;

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
    int ismoving;
    Sprite sprite;
} Paddle;

/* Function prototypes */
static void initsprite(Sprite *s, unsigned int width, unsigned int height,
	unsigned int x, unsigned int y, float rot, const unsigned int *verts,
	size_t size);
static void initbrick(Brick *brick, char id, unsigned int row, unsigned int col);
static void termbrick(Brick *brick);
static unsigned int readbricks(const char *lvl, Brick *bricks);
static void levelload(const char *lvl);
static void initball(void);
static void initpaddle(void);
static void makewall(unsigned int posx, unsigned int posy, unsigned int width,
	unsigned int height);
static void levelunload(void);
static void movepaddle(b2Vec2 vel);
static void movepaddleleft(float frametime);
static void movepaddleright(float frametime);
static void releaseball(float frametime);
static void updatesprite(Sprite *s, b2BodyId id, float dt);
static void resetsmoothstates(void);
static void collisionresolution(void);
static void smoothstates(void);
static void leveldraw(GLuint shader);

/* Variables */
static Brick *bricks;
static unsigned int brickcount;
static Ball ball;
static Paddle paddle;
static GLuint spritesheet, spriteshader, bg;
static Sprite bgsprite;
static b2WorldId worldid;
static int keypressed[GLFW_KEY_LAST + 1];
static const int substepcount = 4; /* Number of sub steps per world step */
static float acc = 0.0f;           /* Time since last world step */
static float alpha = 0.0f;         /* Accumulator ratio for smooth movements */

/* Config uses types from this file */
#include "config.h"

/* Function implementations */

void
initsprite(Sprite *s, unsigned int width, unsigned int height, unsigned int x,
	unsigned int y, float rot, const unsigned int *verts, size_t size)
{
    memcpy(s->texverts, verts, size);
    s->size.x = PIXEL2M(width);
    s->size.y = PIXEL2M(height);
    s->pos.x = PIXEL2M(x);
    s->pos.y = PIXEL2M(y);
    s->rot = rot;
    sprite_init(s);
}

void
initbrick(Brick *brick, char id, unsigned int row, unsigned int col)
{
    Sprite *s = &brick->sprite;
    int solid;
    unsigned int i, x, y;
    b2BodyDef brickbf;
    b2Polygon brickbox;
    b2ShapeDef bricksd;

    solid = (!isdigit(id));
    if (solid)
        i = id - 'a' + bricktypes;
    else
        i = id - '0';

    brick->issolid = solid;
    brick->isdestroyed = 0;

    x  = row * brickwidth;
    y  = scrheight - brickheight - (col * brickheight);
    initsprite(s, brickwidth, brickheight, x, y, 0.0f, brickverts[i],
	    sizeof(brickverts[i]));

    brickbf = b2DefaultBodyDef();
    brickbf.userData = brick;
    brickbf.position = (b2Vec2) {
	PIXEL2M(x + EXT(brickwidth)),
	PIXEL2M(y + EXT(brickheight))
    };
    brick->bodyid = b2CreateBody(worldid, &brickbf);

    brickbox = b2MakeBox(PIXEL2M(EXT(brickwidth)), PIXEL2M(EXT(brickheight)));
    bricksd = b2DefaultShapeDef();
    bricksd.density = 1.0f;
    bricksd.friction = 0.0f;
    bricksd.restitution = 0.0f;
    b2CreatePolygonShape(brick->bodyid, &bricksd, &brickbox);
}

void
termbrick(Brick *brick)
{
    if (!brick->issolid && !brick->isdestroyed) {
	brick->isdestroyed = 1;
	b2DestroyBody(brick->bodyid);
	brick->bodyid = b2_nullBodyId;
    }
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
    initsprite(&ball.sprite, ballwidth, ballheight, 0.0f, 0.0f, 0.0f,
	    ballverts, sizeof(ballverts));

    ballbd = b2DefaultBodyDef();
    ballbd.type = b2_dynamicBody;
    ballbd.userData = &ball;
    ballbd.position = (b2Vec2) {
	PIXEL2M(EXT(scrwidth) - EXT(ballwidth)),
	PIXEL2M(EXT(paddleheight) + ballwidth)
    };
    ball.bodyid = b2CreateBody(worldid, &ballbd);

    ballsd = b2DefaultShapeDef();
    ballsd.density = 1.0f;
    ballsd.friction = 0.0f;
    ballsd.restitution = 1.0f;
    ballsd.enableContactEvents = 1;
    b2CreateCircleShape(ball.bodyid, &ballsd, &circle);
}

void
initpaddle(void)
{
    b2Capsule paddlecap;
    b2BodyDef paddlebd;
    b2ShapeDef paddlesd;

    initsprite(&paddle.sprite, paddlewidth, paddleheight, 0.0f, 0.0f, 0.0f,
	    paddleverts, sizeof(paddleverts));

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
    paddlesd.friction = 0.0f;
    paddlesd.restitution = 0.0f;
    b2CreateCapsuleShape(paddle.bodyid, &paddlesd, &paddlecap);
}

void
makewall(unsigned int posx, unsigned int posy, unsigned int width,
	unsigned int height)
{
    b2BodyDef wallbf;
    b2Polygon wallbox;
    b2ShapeDef wallsd;
    b2BodyId wallid;

    wallbf = b2DefaultBodyDef();
    wallbf.position = (b2Vec2) {
	PIXEL2M(posx + EXT(width)),
	PIXEL2M(posy + EXT(height))
    };
    wallid = b2CreateBody(worldid, &wallbf);

    wallbox = b2MakeBox(PIXEL2M(EXT(width)), PIXEL2M(EXT(height)));
    wallsd = b2DefaultShapeDef();
    wallsd.density = 1.0f;
    wallsd.friction = 0.0f;
    wallsd.restitution = 0.0f;
    b2CreatePolygonShape(wallid, &wallsd, &wallbox);
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

    spritesheet = sprite_load(spritefile, 1);
    bg = sprite_load(bgfile, 1);
    initsprite(&bgsprite, bgwidth, bgheight, 0.0f, 0.0f, 0.0f, bgverts,
	    sizeof(bgverts));
    glActiveTexture(GL_TEXTURE0);
    sprite_shadersetint(spriteshader, texuniform, 0);

    worldDef = b2DefaultWorldDef();
    worldDef.gravity = (b2Vec2) {0.0f, 0.0f};
    worldDef.enableSleep = 0;
    worldid = b2CreateWorld(&worldDef);

    makewall(0, 0, wallwidth, scrheight);
    makewall(wallwidth, scrheight - wallwidth, scrwidth - wallwidth,
	    wallwidth);
    makewall(scrwidth - wallwidth, 0, wallwidth, scrheight - wallwidth);

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
    sprite_unload(spritesheet);
    sprite_unload(bg);
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
movepaddle(b2Vec2 vel)
{
    b2Body_SetLinearVelocity(paddle.bodyid, vel);
}

void
stoppaddle(void)
{
    b2Vec2 vel = { -0.0f, 0.0f };
    movepaddle(vel);
}

void
movepaddleleft(float frametime)
{
    UNUSED(frametime);

    paddle.ismoving = 1;
    b2Vec2 vel = { -5.0f, 0.0f };
    movepaddle(vel);
}

void
movepaddleright(float frametime)
{
    UNUSED(frametime);

    paddle.ismoving = 1;
    b2Vec2 vel = { 5.0f, 0.0f };
    movepaddle(vel);
}

void
releaseball(float frametime)
{
    UNUSED(frametime);
    b2Vec2 force = { -0.5f, 0.5f };

    b2Body_ApplyForceToCenter(ball.bodyid, force, 0);
}

void
game_input(float frametime)
{
    size_t i;

    paddle.ismoving = 0;

    for (i = 0; i < COUNT(keys); i++)
        if (keypressed[keys[i].key])
            (*keys[i].func)(frametime);
}

/* Use interpolation to smooth movement */
void
updatesprite(Sprite *s, b2BodyId id, float dt)
{
    b2Vec2 pos   = b2Body_GetPosition(id);
    b2Rot rot    = b2Body_GetRotation(id);
    b2Vec2 vel   = b2Body_GetLinearVelocity(id);
    float angvel = b2Body_GetAngularVelocity(id);

    s->pos.x = pos.x + vel.x * dt;
    s->pos.y = pos.y + vel.y * dt;

    s->rot = b2Rot_GetAngle(rot) + angvel * dt;
}

void
resetsmoothstates(void)
{
    updatesprite(&ball.sprite, ball.bodyid, 0.0f);
    updatesprite(&paddle.sprite, paddle.bodyid, 0.0f);
}

void
smoothstates(void)
{
    float dt = alpha * timestep;

    updatesprite(&ball.sprite, ball.bodyid, dt);
    updatesprite(&paddle.sprite, paddle.bodyid, dt);
}

void
collisionresolution(void)
{
    int i;
    b2ContactEvents events = b2World_GetContactEvents(worldid);
    b2ContactEndTouchEvent *endevent;
    b2BodyId bodyid;
    void *userdata;
    Brick *brick;

    for (i = 0; i < events.endCount; ++i)
    {
	endevent = events.endEvents + i;

	bodyid = b2Shape_GetBody(endevent->shapeIdA);
	userdata = b2Body_GetUserData(bodyid);
	if (userdata)
	    brick = (Brick *) userdata;
	if (brick)
	    termbrick(brick);
    }
}

void
game_update(float frametime)
{
    unsigned int steps, i;

    if (!paddle.ismoving)
	stoppaddle();

    acc += frametime;
    steps = acc / timestep;

    /* Only update acc when needed, to avoid rounding errors */
    if (steps > 0)
	acc -= steps * timestep;

    alpha = acc / timestep;

    for (i = 0; i < MIN(steps, maxsteps); i++) {
	/* Get actual states for when collision callbacks are fired */
	resetsmoothstates();
	b2World_Step(worldid, timestep, substepcount);
	collisionresolution();
    }

    /* Use smooth states for rendering */
    smoothstates();
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
    sprite_use(bg);
    sprite_draw(spriteshader, &bgsprite);

    sprite_use(spritesheet);
    leveldraw(spriteshader);
    sprite_draw(spriteshader, &ball.sprite);
    sprite_draw(spriteshader, &paddle.sprite);
}
