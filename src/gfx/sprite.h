#pragma once

#include <cglm/struct.h> // vec2s
#include <stdlib.h>      // size_t

// Constants
constexpr size_t IND_COUNT  = 2;  // Number of indices per vertex
constexpr size_t VERT_COUNT = 4;  // Number of vertices per sprite

// Types

typedef struct {
    vec2s pos;
    vec2s texCoord;
} Vert;

typedef struct {
    // First vetex is also the position of the sprite
    union
    {
        vec2s pos;
	Vert verts[VERT_COUNT];
    };
    vec2s size;
} Sprite;

// Function prototypes
Sprite sprite_create(vec2s pos, vec2s size, vec2s texOff, vec2s texSize);
void   sprite_setPos(Sprite* s, vec2s pos);
void   sprite_posAdd(Sprite* s, vec2s v);
void   sprite_texOffAdd(Sprite* s, vec2s v);
bool   sprite_checkCollision(Sprite a, Sprite b);
bool   sprite_checkCollisionEx(Sprite a, Sprite b, vec2s* normal);
float  sprite_sweptAABB(Sprite moving, vec2s movement, Sprite target, vec2s* normal);
