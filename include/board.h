#pragma once

#include <stdint.h>

#include "pos.h"
#include "piece.h"

#define BOARD_SIZE 8

typedef struct {
    Piece pieces[BOARD_SIZE][BOARD_SIZE];
    Pos en_passant;
    bool can_castle_kingside_white;
    bool can_castle_queenside_white;
    bool can_castle_kingside_black;
    bool can_castle_queenside_black;
} Board;

Board board_new_empty(void);
Board board_new_normal(void);
Board board_new_pawnless(void);
Board board_new_castle(void);

Piece board_get(const Board* board, Pos pos);
void board_set(Board* board, Pos pos, Piece piece);
bool board_is_in_bounds(Pos pos);

void board_move(Board* board, Pos from, Pos to);

// If there is a piece at pos, it will be ignored
// The position of the next piece will be written to pos
// Pass in an invalid position (pos_new_invalid()) to start iteration
Piece board_next_piece(const Board* board, Pos* pos);

bool board_can_castle_kingside(const Board* board, PieceColor color);
void board_disable_can_castle_kingside(Board* board, PieceColor color);
bool board_can_castle_queenside(const Board* board, PieceColor color);
void board_disable_can_castle_queenside(Board* board, PieceColor color);
