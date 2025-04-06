#pragma once

// Function prototypes
void level_load(void);
void level_render(Rend* r);
bool level_checkCollision(Sprite ball, vec2s* normal);
