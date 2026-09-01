#include "fonts.h"

Font fonts[FONTS_SIZE] = { 0 };

enum FontIds font_ids;

void fonts_init(void) {
    fonts[FONT_NORMAL] = LoadFont("res/Roboto-Regular.ttf");
    fonts[FONT_NORMAL].baseSize = 24;
}
