#pragma once

#include <stdint.h>

#include "raylib.h"

#define FONTS_SIZE 16

extern Font fonts[FONTS_SIZE];

extern enum FontIds {
    FONT_NORMAL,
} font_ids;

void fonts_init(void);
