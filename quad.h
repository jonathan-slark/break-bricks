#pragma once

#include <cglm/struct.h> // vec2s
#include <stdlib.h>      // size_t

#include "rend.h"

// Constants
constexpr size_t IND_COUNT  = 2  // Number of indices per vertex
constexpr size_t VERT_COUNT = 4  // Number of vertices per quad

// Types

typedef struct {
    vec2s pos;
    vec2s texCoord;
} Vert;

typedef struct {
    Vert verts[VERT_COUNT];
} Quad;

// Function prototypes
Quad quad_create(Rend* r, vec2s pos, vec2s size, vec2s texOffset);
void quad_setPos(Quad* q, vec2s pos, vec2s size);
void quad_addVec(Quad* q, vec2s v);
