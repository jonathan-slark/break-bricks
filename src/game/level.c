#include <ctype.h> // isdigit

#include "../main.h"
#include "../util.h"
#include "../gfx/rend.h"
#include "../gfx/sprite.h"
#include "brick.h"

// Function prototypes
static void levelRead(int level, const char *data);

// Constants
static const char FOLDER[] = "level";
constexpr int COUNT = 7;
constexpr int COLS  = 12;
constexpr int ROWS  = 24;

// Variables
static Brick levels[COUNT][COLS * ROWS];

// Function declarations

void levelRead(int level, const char *data)
{
    int count = 0;
    int col   = 0;
    int row   = 0;

    char c = '\0';
    while ((c = *data++) != '\0')
    {
        if (c == '#')
        {
            while ((c = *data++) != '\n')
            {
                // Comment
            }
        }
        else if (c == 'x')
        {
            // No brick
            levels[level][count++].isActive = false;
            col++;
        }
        else if (isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
        {
            levels[level][count++] = brick_create(c, col, row);
            col++;
        }
        else if (c == '\n')
        {
            // Ignore blank line
            if (col > 0) row++;
            col = 0;
        }
        else if (c != ' ' && c != '\t')
        {
            main_term(EXIT_FAILURE, "Syntax error in level file.\n");
        }
    }

    if (count != COLS * ROWS)
    {
        main_term(EXIT_FAILURE, "Incorrect number of bricks in level file.\n");
    }
}

void level_load(void)
{
    for (int level = 1; level <= COUNT; level++)
    {
	char fmt[] = "%s/%02i.txt";
	int size = snprintf(nullptr, 0, fmt, FOLDER, level);
	char file[size + 1];
	snprintf(file, sizeof file, fmt, FOLDER, level);
	char *data = util_load(file, READ_ONLY_TEXT);
	if (!data) main_term(EXIT_FAILURE, "Unable to load level:%s\n", file);

	levelRead(level - 1, data);

	util_unload(data);
    }
}

void level_render(int level, Rend* r)
{
    for (int i = 0; i < COLS * ROWS; i++)
    {
        Brick b = levels[level][i];
        if (b.isActive && !b.isDestroyed)
        {
            rend_sprite(r, b.sprite);
        }
    }
}
