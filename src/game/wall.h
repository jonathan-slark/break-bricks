#pragma once

// Types
typedef enum
{
    WallTop,
    WallLeft,
    WallRight,
    WallCount
} Wall;

// Constants
extern const float WALL_TOP;
extern const float WALL_LEFT;
extern const float WALL_RIGHT;

// Function prototypes
void   wall_init(void);
Sprite wall_getSprite(Wall w);
