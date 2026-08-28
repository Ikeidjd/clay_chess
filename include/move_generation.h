#pragma once

#include "dir.h"
#include "board.h"
#include "move.h"

typedef struct {
    Move moves[BOARD_SIZE][BOARD_SIZE];
} MoveBoard;

Move move_board_get(MoveBoard* moves, Pos pos);
void move_board_set(MoveBoard* moves, Pos pos, Move move);

void moves_generate(Board* board, MoveBoard* moves, Pos pos);
