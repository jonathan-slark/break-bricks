#include <ctype.h> // isdigit

#include "../main.h"
#include "../util.h"
#include "../gfx/rend.h"
#include "../gfx/sprite.h"
#include "audio.h"
#include "paddle.h"
#include "wall.h"

// Types
typedef struct
{
    bool   isActive;
    bool   isSolid;
    bool   isDestroyed;
    Sprite sprite;
} Brick;

// Function prototypes
static void  readLevel(int level, const char *data);
static Brick createBrick(char id, int col, int row);
static void  updateScore(int i);

// Constants
static const char FOLDER[] = "level";
constexpr int COUNT = 7;
constexpr int COLS  = 12;
constexpr int ROWS  = 24;
static const vec2s    SIZE = {{ 128, 32 }};
static const vec2s    NORMAL_OFFSETS[] = {
    {{ 0,   64 }}, // blue,   id = 0
    {{ 128, 64 }}, // green,  id = 1
    {{ 256, 64 }}, // orange, id = 2
    {{ 0,   96 }}, // purple, id = 3
    {{ 128, 96 }}, // red,    id = 4
    {{ 256, 96 }}  // yellow, id = 5
};
static const vec2s    SOLID_OFFSETS[] = {
    {{ 0,   0 }},  // blue,   id = a
    {{ 128, 0 }},  // green,  id = b
    {{ 256, 0 }},  // orange, id = c
    {{ 0,   32 }}, // purple, id = d
    {{ 128, 32 }}, // red,    id = e
    {{ 256, 32 }}, // yellow, id = f
};

// Variables
static Brick levels[COUNT][COLS * ROWS];
static int level;

// Function definitions

Brick createBrick(char id, int col, int row)
{
    Brick b;

    bool isSolid = !isdigit(id);

    b.isActive    = true;
    b.isSolid     = isSolid;
    b.isDestroyed = false;

    vec2s pos    = {{ WALL_LEFT + col * SIZE.s, WALL_TOP + row * SIZE.t }};
    vec2s offset = isSolid ? SOLID_OFFSETS[id - 'a'] : NORMAL_OFFSETS[id - '0'];
    b.sprite     = sprite_create(pos, SIZE, offset, (vec2s) {{ SCR_WIDTH, SCR_HEIGHT }});

    return b;
}

void readLevel(int level, const char *data)
{
    int count = 0;
    int col   = 0;
    int row   = 0;

    char c = '\0';
    while ((c = *data++) != '\0') {
        if (c == '#') {
            while ((c = *data++) != '\n') {
                // Comment
            }
        } else if (c == 'x') {
            // No brick
            levels[level][count++].isActive = false;
            col++;
        } else if (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')) {
            levels[level][count++] = createBrick(c, col, row);
            col++;
        } else if (c == '\n') {
            // Ignore blank line
            if (col > 0) row++;
            col = 0;
        } else if (c != ' ' && c != '\t') {
            main_term(EXIT_FAILURE, "Syntax error in level file.\n");
        }
    }

    if (count != COLS * ROWS) {
        main_term(EXIT_FAILURE, "Incorrect number of bricks in level file.\n");
    }
}

void level_load(void)
{
    // Files are labeled 1 to COUNT but array is indexed as 0 to COUNT-1
    for (int i = 1; i <= COUNT; i++) {
	char fmt[] = "%s/%02i.txt";
	int size = snprintf(nullptr, 0, fmt, FOLDER, i);
	char file[size + 1];
	snprintf(file, sizeof file, fmt, FOLDER, i);
	char *data = util_load(file, READ_ONLY_TEXT);
	if (!data) main_term(EXIT_FAILURE, "Unable to load level:%s\n", file);

	readLevel(i - 1, data);

	util_unload(data);
    }

    level = 0;
}

void level_rend(Rend* r)
{
    for (int i = 0; i < COLS * ROWS; i++) {
        Brick b = levels[level][i];
        if (b.isActive && !b.isDestroyed) {
            rend_sprite(r, b.sprite);
        }
    }
}

bool level_isClear(void)
{
    for (int i = 0; i < COLS * ROWS; i++) {
        Brick b = levels[level][i];
        if (b.isActive && !b.isSolid && !b.isDestroyed) return false;
    }

    return true;
}

// Reset all levels
void level_reset(void)
{
    for (int j = 0; j < COUNT; j++) {
	for (int i = 0; i < COLS * ROWS; i++) {
	    Brick* b = &levels[j][i];
	    if (b->isActive && !b->isSolid && b->isDestroyed) b->isDestroyed = false;
	}
    }

    level = 0;
}

// Returns false if game is won
bool level_next(void)
{
    if (level < COUNT - 1) {
	level++;
	return true;
    } else {
	return false;
    }
}

int level_get(void)
{
    return level;
}

int level_getBrickCount(void)
{
    return COLS * ROWS;
}

// Return a pointer to a sprite, nullptr means no brick
Sprite* level_getBrickSprite(int brick)
{
    Brick* b = &levels[level][brick];
    if (b->isActive && !b->isDestroyed) {
	return &b->sprite;
    } else {
	return nullptr;
    }
}

void updateScore(int i)
{
    int row = i / COLS;
    paddle_incScore((level + 1) * (ROWS - row));
}

void level_destroyBrick(int brick)
{
    Brick* b = &levels[level][brick];
    if (!b->isSolid) {
	b->isDestroyed = true;
	updateScore(brick);
	audio_playSound(SoundBrick);
    }
}
