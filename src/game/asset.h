#pragma once

#include "../gfx/font.h"
#include "../gfx/screen.h"
#include "text.h"

// Function prototypes
void   asset_loading(void);
void   asset_load(void);
Screen asset_getLoading(void);
Screen asset_getBg(void);
Rend*  asset_getSpriteRend(void);
Font*  asset_getFont(FontSize size);
