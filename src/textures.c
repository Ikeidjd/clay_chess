#include "textures.h"

struct Textures textures;

#define LOAD(name) LoadTexture("res/textures/" name ".png")

void textures_init(void) {
    textures.pieces[PIECE_COLOR_WHITE][PIECE_TYPE_PAWN] = LOAD("white/pawn");
    textures.pieces[PIECE_COLOR_WHITE][PIECE_TYPE_KNIGHT] = LOAD("white/knight");
    textures.pieces[PIECE_COLOR_WHITE][PIECE_TYPE_BISHOP] = LOAD("white/bishop");
    textures.pieces[PIECE_COLOR_WHITE][PIECE_TYPE_ROOK] = LOAD("white/rook");
    textures.pieces[PIECE_COLOR_WHITE][PIECE_TYPE_QUEEN] = LOAD("white/queen");
    textures.pieces[PIECE_COLOR_WHITE][PIECE_TYPE_KING] = LOAD("white/king");

    textures.pieces[PIECE_COLOR_BLACK][PIECE_TYPE_PAWN] = LOAD("black/pawn");
    textures.pieces[PIECE_COLOR_BLACK][PIECE_TYPE_KNIGHT] = LOAD("black/knight");
    textures.pieces[PIECE_COLOR_BLACK][PIECE_TYPE_BISHOP] = LOAD("black/bishop");
    textures.pieces[PIECE_COLOR_BLACK][PIECE_TYPE_ROOK] = LOAD("black/rook");
    textures.pieces[PIECE_COLOR_BLACK][PIECE_TYPE_QUEEN] = LOAD("black/queen");
    textures.pieces[PIECE_COLOR_BLACK][PIECE_TYPE_KING] = LOAD("black/king");

    textures.move_indicator = LOAD("move_indicator");
    textures.capture_indicator = LOAD("capture_indicator");
}
