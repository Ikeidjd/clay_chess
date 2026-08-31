#include "piece.h"

#include <stdlib.h>

#include "raylib.h"

#include "textures.h"

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
    return piece_is_empty(piece) ? NULL : &textures.pieces[piece.color][piece.type];
}

char piece_get_notation(Piece piece) {
    switch (piece.type) {
        case PIECE_TYPE_EMPTY:
            return ' ';
        case PIECE_TYPE_PAWN:
            return '\0';
        case PIECE_TYPE_KNIGHT:
            return 'N';
        case PIECE_TYPE_BISHOP:
            return 'B';
        case PIECE_TYPE_ROOK:
            return 'R';
        case PIECE_TYPE_QUEEN:
            return 'Q';
        case PIECE_TYPE_KING:
            return 'K';
    }
}
