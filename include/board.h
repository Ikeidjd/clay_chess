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
