#pragma once

#include <stdint.h>

#include "pos.h"
#include "piece.h"

#define BOARD_SIZE 8

typedef struct {
    Piece pieces[BOARD_SIZE][BOARD_SIZE];
} Board;

Board board_new_normal(void);
Board board_new_pawnless(void);
Board board_new_castle(void);

Piece board_get(const Board* board, Pos pos);
void board_set(Board* board, Pos pos, Piece piece);
bool board_is_in_bounds(Pos pos);

void board_move(Board* board, Pos from, Pos to);

// If there is a piece at pos, it will be ignored
// The position of the next_piece will be written to pos
// Pass in an invalid position (pos_new_invalid()) to start iteration
Piece board_next_piece(const Board* board, Pos* pos);
