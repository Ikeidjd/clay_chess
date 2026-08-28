#include "pos.h"

#include "board.h"

Pos pos_new_invalid(void) {
    return (Pos) { -1, -1 };
}

Pos pos_plus_dir(Pos pos, Dir dir) {
    return (Pos) { pos.row + dir.row, pos.col + dir.col };
}

bool pos_is_valid(Pos pos) {
    return pos.row < BOARD_SIZE && pos.col < BOARD_SIZE;
}

bool pos_eq(Pos left, Pos right) {
    return left.row == right.row && left.col == right.col;
}
