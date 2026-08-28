#pragma once

#include "pos.h"
#include "piece.h"
#include "board.h"

typedef enum {
    MOVE_EMPTY,
    MOVE_NORMAL,
    MOVE_CASTLE,
    MOVE_PROMOTION,
} MoveType;

typedef struct {
    Pos from;
    Pos to;
} MoveNormal;

typedef struct {
    MoveNormal king_move;
    MoveNormal rook_move;
} MoveCastle;

typedef struct {
    MoveNormal move;
    Piece transgender;
} MovePromotion;

typedef struct {
    MoveType type;
    union {
        MoveNormal normal;
        MoveCastle castle;
        MovePromotion promotion;
    } as;
} Move;

Move move_normal_as_move(MoveNormal move);
Move move_castle_as_move(MoveCastle move);
Move move_promotion_as_move(MovePromotion move);

Move move_new_empty();
Move move_new_normal(Pos from, Pos to);
Move move_new_castle(MoveNormal king_move, MoveNormal rook_move);
Move move_new_promotion(MoveNormal move, Piece transgender);

bool move_is_empty(Move move);

void move_execute(Board* board, Move move);
