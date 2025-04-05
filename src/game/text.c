#include <stdarg.h> // va_list, va_start, va_end

#include "../gfx/font.h"
#include "asset.h"
#include "text.h"

// Function definitions

void text_rend(Text t, ...)
{
    Font* f = asset_getFont(t.size);

    va_list args;
    va_start(args, t);

    font_begin(*f);
    font_vprintf(f, t.pos, t.col, t.fmt, args);
    font_end(f);

    va_end(args);
}
