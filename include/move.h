#pragma once

#include "pos.h"
#include "piece.h"
#include "board.h"

typedef enum {
    MOVE_EMPTY,
    MOVE_NORMAL,
    MOVE_CASTLE,
    MOVE_PAWN_DOUBLE,
    MOVE_EN_PASSANT,
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
        MoveNormal pawn_double;
        MoveNormal en_passant;
        MovePromotion promotion;
    } as;
} Move;

Move move_normal_wrap(MoveNormal move);
Move move_castle_wrap(MoveCastle move);
Move move_pawn_double_wrap(MoveNormal move);
Move move_en_passant_wrap(MoveNormal move);
Move move_promotion_wrap(MovePromotion move);

Move move_new_empty(void);
Move move_new_normal(Pos from, Pos to);
Move move_new_castle(MoveNormal king_move, MoveNormal rook_move);
Move move_new_pawn_double(Pos from, Pos to);
Move move_new_en_passant(Pos from, Pos to);
Move move_new_promotion(MoveNormal move, Piece transgender);

bool move_is_empty(Move move);
bool move_is_legal(Board board, Move move, PieceColor king_color);

void move_execute(Board* board, Move move);

#define MOVE_NOTATION_SIZE 16

typedef struct {
    char data[MOVE_NOTATION_SIZE];
} MoveNotation;

MoveNotation move_get_notation(const Board* board, Move move);
