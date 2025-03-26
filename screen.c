#include <cglm/struct.h> // vec2s

#include "quad.h"
#include "rend.h"
#include "screen.h"

void screen_init(Screen *s, const char *file)
{
    vec2s pos  = (vec2s) {{ 0, 0 }};
    vec2s size = (vec2s) {{ SCR_WIDTH, SCR_HEIGHT }};
    s->rend    = rend_create(1, file);
    s->quad    = quad_create(&s->rend, pos, size, pos);
}

void screen_rend(Screen *s)
{
    rend_begin(&s->rend);
    rend_quad(&s->rend, &s->quad);
    rend_end(&s->rend);
}
