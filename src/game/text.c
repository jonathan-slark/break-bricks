#include "../gfx/font.h"
#include "text.h"

// Function declarations

void text_rend(Font* f, Text t)
{
    font_begin(*f);
    font_printf(f, t.pos, t.col, t.fmt);
    font_end(f);
}
