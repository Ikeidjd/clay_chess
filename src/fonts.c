#include "fonts.h"

#include <stddef.h>

Font fonts[FONTS_SIZE] = { 0 };

enum FontIds font_ids;

void fonts_init(void) {
    fonts[FONT_NORMAL] = LoadFontEx("res/Roboto-Regular.ttf", 96, NULL, 0);
    SetTextureFilter(fonts[FONT_NORMAL].texture, TEXTURE_FILTER_BILINEAR);
}
