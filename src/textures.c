#include "textures.h"

struct Textures textures;

void textures_init(void) {
    textures.pieces[PIECE_COLOR_WHITE][PIECE_TYPE_PAWN] = LoadTexture("res/white/pawn.png");
    textures.pieces[PIECE_COLOR_WHITE][PIECE_TYPE_KNIGHT] = LoadTexture("res/white/knight.png");
    textures.pieces[PIECE_COLOR_WHITE][PIECE_TYPE_BISHOP] = LoadTexture("res/white/bishop.png");
    textures.pieces[PIECE_COLOR_WHITE][PIECE_TYPE_ROOK] = LoadTexture("res/white/rook.png");
    textures.pieces[PIECE_COLOR_WHITE][PIECE_TYPE_QUEEN] = LoadTexture("res/white/queen.png");
    textures.pieces[PIECE_COLOR_WHITE][PIECE_TYPE_KING] = LoadTexture("res/white/king.png");

    textures.pieces[PIECE_COLOR_BLACK][PIECE_TYPE_PAWN] = LoadTexture("res/black/pawn.png");
    textures.pieces[PIECE_COLOR_BLACK][PIECE_TYPE_KNIGHT] = LoadTexture("res/black/knight.png");
    textures.pieces[PIECE_COLOR_BLACK][PIECE_TYPE_BISHOP] = LoadTexture("res/black/bishop.png");
    textures.pieces[PIECE_COLOR_BLACK][PIECE_TYPE_ROOK] = LoadTexture("res/black/rook.png");
    textures.pieces[PIECE_COLOR_BLACK][PIECE_TYPE_QUEEN] = LoadTexture("res/black/queen.png");
    textures.pieces[PIECE_COLOR_BLACK][PIECE_TYPE_KING] = LoadTexture("res/black/king.png");

    textures.move_indicator = LoadTexture("res/move_indicator.png");
    textures.capture_indicator = LoadTexture("res/capture_indicator.png");
}
