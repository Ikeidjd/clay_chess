#pragma once

#include "raylib.h"

#include "piece.h"

extern struct Textures {
    Texture2D pieces[PIECE_COLOR_BLACK + 1][PIECE_TYPE_KING + 1];
    Texture2D move_indicator;
    Texture2D capture_indicator;
} textures;

void textures_init(void);
