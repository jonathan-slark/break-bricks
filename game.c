/*
 * This file is released into the public domain under the CC0 1.0 Universal License.
 * For details, see https://creativecommons.org/publicdomain/zero/1.0/
 */

#define GLFW_INCLUDE_NONE
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
#define PIXEL2M(x) ((x) / (double)paddlewidth)
#define EXT(x) ((x) / 2.0f)

/* Types */

typedef struct
{
    int key;
    void (*func)(double frametime);
} Key;

typedef struct
{
    bool isstuck;
    Sprite sprite;
} Ball;

typedef struct
{
    bool issolid;
    bool isdestroyed;
    Sprite sprite;
} Brick;

/* Function prototypes */
static void initsprite(Sprite *s, unsigned width, unsigned height, unsigned x, unsigned y, double rot, const unsigned *verts, size_t size);
static void initbrick(Brick *brick, char id, unsigned row, unsigned col);
static void termbrick(Brick *brick);
static unsigned readbricks(const char *lvl, Brick *bricks);
static void levelload(const char *lvl);
static void initball(void);
static void initpaddle(void);
static void levelunload(void);
static void movepaddle(double vel);
static void movepaddleleft(double frametime);
static void movepaddleright(double frametime);
static void releaseball(double frametime);
static void leveldraw(GLuint shader);

/* Variables */
static Brick *bricks = NULL;
static unsigned brickcount = 0;
static Ball ball = {0};
static Sprite paddle = {0};
static GLuint spritesheet = 0, spriteshader = 0, bg = 0;
static Sprite bgsprite = {0};
static bool keypressed[GLFW_KEY_LAST + 1] = {0};

/* Config uses types from this file */
#include "config.h"

/* Function implementations */

void initsprite(Sprite *s, unsigned width, unsigned height, unsigned x, unsigned y, double rot, const unsigned *verts, size_t size)
{
    memcpy(s->texverts, verts, size);
    s->size.x = PIXEL2M(width);
    s->size.y = PIXEL2M(height);
    s->pos.x = PIXEL2M(x);
    s->pos.y = PIXEL2M(y);
    s->rot = rot;
    sprite_init(s);
}

void initbrick(Brick *brick, char id, unsigned row, unsigned col)
{
    bool solid = (!isdigit(id));
    unsigned i;
    if (solid)
        i = id - 'a' + bricktypes;
    else
        i = id - '0';

    brick->issolid = solid;
    brick->isdestroyed = 0;

    unsigned x = row * brickwidth;
    unsigned y = scrheight - brickheight - (col * brickheight);
    initsprite(&brick->sprite, brickwidth, brickheight, x, y, 0.0f, brickverts[i], sizeof(brickverts[i]));
}

void termbrick(Brick *brick)
{
    if (!brick->issolid && !brick->isdestroyed)
        brick->isdestroyed = 1;
}

/* Run once with bricks = NULL to get the brick count, a second time with
 * bricks pointing to an array of bricks to be initialised. */
unsigned readbricks(const char *lvl, Brick *bricks)
{
    unsigned count = 0, row = 0, col = 0;

    char c = '\0';
    while ((c = *lvl++) != '\0')
    {
        if (c == '#')
        {
            while ((c = *lvl++) != '\n')
            {
                /* Comment */
            }
        }
        else if (c == 'x')
        {
            /* No brick */
            row++;
        }
        else if (isdigit(c) || (c >= 'a' && c <= 'f'))
        {
            if (bricks)
                initbrick(&bricks[count], c, row, col);
            count++;
            row++;
        }
        else if (c == '\n')
        {
            /* Ignore blank line */
            if (row > 0)
                col++;
            row = 0;
        }
        else if (c != ' ' && c != '\t')
        {
            term(EXIT_FAILURE, "Syntax error in level file.\n");
        }
    }

    return count;
}

void levelload(const char *name)
{
    char *lvl = load(name);
    brickcount = readbricks(lvl, NULL);
    bricks = (Brick *)malloc(brickcount * sizeof(Brick));
    readbricks(lvl, bricks);
    unload(lvl);
}

void initball(void)
{
    ball.isstuck = 1;
    double x = scrwidth / 2.0f - ballwidth / 2.0f;
    double y = scrheight / 2.0f - ballheight / 2.0f;
    initsprite(&ball.sprite, ballwidth, ballheight, x, y, 0.0f, ballverts, sizeof(ballverts));
}

void initpaddle(void)
{
    double x = scrwidth / 2.0f - paddlewidth / 2.0f;
    double y = scrheight - paddleheight;
    initsprite(&paddle, paddlewidth, paddleheight, x, y, 0.0f, paddleverts, sizeof(paddleverts));
}

void game_load(void)
{
    mat4s proj = glms_ortho(0.0f, PIXEL2M(scrwidth), 0.0f, PIXEL2M(scrheight), -1.0f, 1.0f);
    spriteshader = sprite_shaderload(vertshader, fragshader);
    sprite_shaderuse(spriteshader);
    sprite_shadersetmat4s(spriteshader, projuniform, proj);

    spritesheet = sprite_load(spritefile, 1);
    bg = sprite_load(bgfile, 1);
    initsprite(&bgsprite, bgwidth, bgheight, 0.0f, 0.0f, 0.0f, bgverts, sizeof(bgverts));
    glActiveTexture(GL_TEXTURE0);
    sprite_shadersetint(spriteshader, texuniform, 0);

    char lvl[] = LVLFOLDER "/00.txt";
    char fmt[] = LVLFOLDER "/%02i.txt";
    sprintf(lvl, fmt, 1);
    levelload(lvl);
    initball();
    initpaddle();
}

void levelunload(void)
{
    for (unsigned i = 0; i < brickcount; i++)
        sprite_term(&bricks[i].sprite);

    free(bricks);
    brickcount = 0;
}

void game_unload(void)
{
    sprite_term(&paddle);
    sprite_term(&ball.sprite);
    levelunload();
    sprite_unload(spritesheet);
    sprite_unload(bg);
    sprite_shaderunload(spriteshader);
}

void game_keydown(int key)
{
    keypressed[key] = true;
}

void game_keyup(int key)
{
    keypressed[key] = false;
}

void movepaddle(double vel)
{
}

void movepaddleleft(double frametime)
{
    UNUSED(frametime);

    movepaddle(-5.0f);
}

void movepaddleright(double frametime)
{
    UNUSED(frametime);

    movepaddle(5.0f);
}

void releaseball(double frametime)
{
    if (!ball.isstuck)
        return;

    UNUSED(frametime);

    ball.isstuck = 0;
}

void game_input(double frametime)
{
    for (size_t i = 0; i < COUNT(keys); i++)
        if (keypressed[keys[i].key])
            (*keys[i].func)(frametime);
}

void game_update(double frametime)
{

}

void leveldraw(GLuint shader)
{
    for (unsigned i = 0; i < brickcount; i++)
        if (!bricks[i].isdestroyed)
            sprite_draw(shader, &bricks[i].sprite);
}

void game_render(void)
{
    sprite_use(bg);
    sprite_draw(spriteshader, &bgsprite);

    sprite_use(spritesheet);
    leveldraw(spriteshader);
    sprite_draw(spriteshader, &ball.sprite);
    sprite_draw(spriteshader, &paddle);
}
