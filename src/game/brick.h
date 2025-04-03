#include "../gfx/sprite.h"

typedef struct
{
    bool   isActive;
    bool   isSolid;
    bool   isDestroyed;
    Sprite sprite;
} Brick;

Brick brick_create(char id, int col, int row);
