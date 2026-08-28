#include "piece.h"

#include <stdlib.h>

#include "raylib.h"

static Texture2D piece_textures[PIECE_COLOR_BLACK + 1][PIECE_TYPE_KING + 1];

void piece_textures_init(void) {
    piece_textures[PIECE_COLOR_WHITE][PIECE_TYPE_PAWN] = LoadTexture("res/white/pawn.png");
    piece_textures[PIECE_COLOR_WHITE][PIECE_TYPE_KNIGHT] = LoadTexture("res/white/knight.png");
    piece_textures[PIECE_COLOR_WHITE][PIECE_TYPE_BISHOP] = LoadTexture("res/white/bishop.png");
    piece_textures[PIECE_COLOR_WHITE][PIECE_TYPE_ROOK] = LoadTexture("res/white/rook.png");
    piece_textures[PIECE_COLOR_WHITE][PIECE_TYPE_QUEEN] = LoadTexture("res/white/queen.png");
    piece_textures[PIECE_COLOR_WHITE][PIECE_TYPE_KING] = LoadTexture("res/white/king.png");

    piece_textures[PIECE_COLOR_BLACK][PIECE_TYPE_PAWN] = LoadTexture("res/black/pawn.png");
    piece_textures[PIECE_COLOR_BLACK][PIECE_TYPE_KNIGHT] = LoadTexture("res/black/knight.png");
    piece_textures[PIECE_COLOR_BLACK][PIECE_TYPE_BISHOP] = LoadTexture("res/black/bishop.png");
    piece_textures[PIECE_COLOR_BLACK][PIECE_TYPE_ROOK] = LoadTexture("res/black/rook.png");
    piece_textures[PIECE_COLOR_BLACK][PIECE_TYPE_QUEEN] = LoadTexture("res/black/queen.png");
    piece_textures[PIECE_COLOR_BLACK][PIECE_TYPE_KING] = LoadTexture("res/black/king.png");
}

Piece piece_new_empty(void) {
    return (Piece) {
        .type = PIECE_TYPE_EMPTY,
        .color = PIECE_COLOR_EMPTY,
    };
}

bool piece_is_empty(Piece piece) {
    return piece.type == PIECE_TYPE_EMPTY || piece.color == PIECE_COLOR_EMPTY;
}

Texture2D* piece_get_texture(Piece piece) {
    return piece_is_empty(piece) ? NULL : &piece_textures[piece.color][piece.type];
}
