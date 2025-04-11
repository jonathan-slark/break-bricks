#include <stdarg.h> // va_list, va_start, va_end

#include "../gfx/font.h"
#include "asset.h"
#include "text.h"

// Function definitions

static Font* currentFont = NULL;

void text_rend(Text t, ...)
{
    Font* f = asset_getFont(t.size);

    if (currentFont != f) {
        if (currentFont) font_end(currentFont);
        font_begin(*f, t.col);
        currentFont = f;
    }

    va_list args;
    va_start(args, t);

    font_vprintf(f, t.pos, t.fmt, args);

    va_end(args);
}

void text_flush()
{
    if (currentFont) {
        font_end(currentFont);
        currentFont = NULL;
    }
}
