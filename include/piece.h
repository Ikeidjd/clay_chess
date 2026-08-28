#pragma once

#include <stdbool.h>

#include "raylib.h"

typedef enum {
    PIECE_TYPE_EMPTY,
    PIECE_TYPE_PAWN,
    PIECE_TYPE_KNIGHT,
    PIECE_TYPE_BISHOP,
    PIECE_TYPE_ROOK,
    PIECE_TYPE_QUEEN,
    PIECE_TYPE_KING,
} PieceType;

typedef enum {
    PIECE_COLOR_EMPTY,
    PIECE_COLOR_WHITE,
    PIECE_COLOR_BLACK,
} PieceColor;

typedef struct {
    PieceType type;
    PieceColor color;
} Piece;

void piece_textures_init(void);
Piece piece_new_empty(void);

bool piece_is_empty(Piece piece);
Texture2D* piece_get_texture(Piece piece);
