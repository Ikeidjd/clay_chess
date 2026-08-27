#include "board.h"

#include <assert.h>

Piece board_get(Board* board, size_t i, size_t j) {
    assert(i < BOARD_SIZE && j < BOARD_SIZE);
    return board->pieces[i][j];
}

void board_set(Board* board, size_t i, size_t j, Piece piece) {
    assert(i < BOARD_SIZE && j < BOARD_SIZE);
    board->pieces[i][j] = piece;
}
