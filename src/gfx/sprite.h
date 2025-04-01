#pragma once

#include <cglm/struct.h> // vec2s
#include <stdlib.h>      // size_t

// Constants
constexpr size_t IND_COUNT  = 2;  // Number of indices per vertex
constexpr size_t VERT_COUNT = 4;  // Number of vertices per sprite

// Types

typedef struct
{
    vec2s pos;
    vec2s texCoord;
} Vert;

typedef struct
{
    // First vetex is also the position of the sprite
    union
    {
        vec2s pos;
	Vert verts[VERT_COUNT];
    };
    vec2s size;
} Sprite;

// Function prototypes
Sprite sprite_create(vec2s pos, vec2s size, vec2s texOffset, vec2s texSize);
void   sprite_setPos(Sprite* s, vec2s pos, vec2s size);
void   sprite_addVec(Sprite* s, vec2s v);
