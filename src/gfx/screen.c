#include <cglm/struct.h> // vec2s

#include "../main.h"
#include "rend.h"
#include "screen.h"
#include "sprite.h"

// Function definitions

Screen screen_load(const char *file)
{
    Screen s;
    vec2s pos  = (vec2s) {{ 0, 0 }};
    vec2s size = (vec2s) {{ SCR_WIDTH, SCR_HEIGHT }};
    s.rend     = rend_load(1, file);
    s.sprite   = sprite_create(pos, size, pos, size);
    return s;
}

void screen_unload(Screen s)
{
    rend_unload(s.rend);
}

void screen_rend(Screen s)
{
    rend_begin(s.rend);
    rend_sprite(&s.rend, s.sprite);
    rend_end(&s.rend);
}
