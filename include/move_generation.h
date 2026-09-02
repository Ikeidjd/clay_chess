#pragma once

#include <stddef.h>

#include "dir.h"
#include "board.h"
#include "move.h"

// A move array is a sequence of moves in an arbitrary order
// It's used when the only thing needed is iterating through the moves (check detection, bot, etc.)
typedef struct {
    Move moves[BOARD_SIZE * BOARD_SIZE];
    size_t count;
} MoveArray;

MoveArray move_array_new(void);

Move move_array_get(const MoveArray* moves, size_t i);
void move_array_insert(MoveArray* moves, Move move);

// A move board is a sequence of moves that can be indexed with a position
// It's used for user input: a square is clicked, its position is obtained and it's used as an index
typedef struct {
    Move moves[BOARD_SIZE][BOARD_SIZE];
} MoveBoard;

MoveBoard move_board_new(void);

Move move_board_get(const MoveBoard* moves, Pos pos);
void move_board_set(MoveBoard* moves, Pos pos, Move move);

MoveBoard move_array_to_board(const MoveArray* array);

MoveArray moves_generate(const Board* board, Pos pos, bool can_castle_kingside, bool can_castle_queenside);
MoveBoard moves_generate_board(const Board* board, Pos pos, bool can_castle_kingside, bool can_castle_queenside);
