#include "fonts.h"

#include <stddef.h>

Font fonts[FONTS_SIZE] = { 0 };

enum FontIds font_ids;

#define LOAD(id, name) \
do { \
    fonts[id] = LoadFontEx("res/fonts/" name ".ttf", 96, NULL, 0); \
    SetTextureFilter(fonts[id].texture, TEXTURE_FILTER_BILINEAR); \
} while(0)

void fonts_init(void) {
    LOAD(FONT_NORMAL, "Roboto-Regular");
}
