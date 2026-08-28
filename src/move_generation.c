#include "move_generation.h"

#include <assert.h>

#include "piece.h"

Move move_board_get(const MoveBoard* moves, Pos pos) {
    assert(pos.row < BOARD_SIZE && pos.col < BOARD_SIZE);
    return moves->moves[pos.row][pos.col];
}

void move_board_set(MoveBoard* moves, Pos pos, Move move) {
    assert(pos.row < BOARD_SIZE && pos.col < BOARD_SIZE);
    moves->moves[pos.row][pos.col] = move;
}

static void moves_generate_single(const Board* board, MoveBoard* moves, Pos from, Dir* dirs, size_t dirs_length) {
    Piece src = board_get(board, from);

    for (size_t i = 0; i < dirs_length; i++) {
        Dir dir = dirs[i];
        Pos to = pos_plus_dir(from, dir);

        if (!board_is_in_bounds(to)) continue;

        Piece dst = board_get(board, to);

        if (src.color != dst.color) {
            move_board_set(moves, to, move_new_normal(from, to));
        }
    }
}

static void moves_generate_sliding(const Board* board, MoveBoard* moves, Pos from, Dir* dirs, size_t dirs_length) {
    Piece src = board_get(board, from);

    for (size_t i = 0; i < dirs_length; i++) {
        Dir dir = dirs[i];
        Pos to = pos_plus_dir(from, dir);

        while (board_is_in_bounds(to) && piece_is_empty(board_get(board, to))) {
            move_board_set(moves, to, move_new_normal(from, to));
            to = pos_plus_dir(to, dir);
        }

        if (board_is_in_bounds(to) && src.color != board_get(board, to).color) {
            move_board_set(moves, to, move_new_normal(from, to));
        }
    }
}

static Dir knight_dirs[] = {
    { 2, 1 },
    { 2, -1 },
    { -2, 1 },
    { -2, -1 },
    { 1, 2 },
    { -1, 2 },
    { 1, -2 },
    { -1, -2 },
};

static Dir diagonal_dirs[] = {
    { 1, 1 },
    { 1, -1 },
    { -1, 1 },
    { -1, -1 },
};

static Dir orthogonal_dirs[] = {
    { 1, 0 },
    { -1, 0 },
    { 0, 1 },
    { 0, -1 },
};

static Dir cardinal_dirs[] = {
    { 1, 1 },
    { 1, -1 },
    { -1, 1 },
    { -1, -1 },
    { 1, 0 },
    { -1, 0 },
    { 0, 1 },
    { 0, -1 },
};

void moves_generate(const Board* board, MoveBoard* moves, Pos pos) {
    *moves = (MoveBoard) { 0 };
    Piece piece = board_get(board, pos);

    switch (piece.type) {
        case PIECE_TYPE_EMPTY:
            break;
        case PIECE_TYPE_PAWN:
            break;
        case PIECE_TYPE_KNIGHT:
            moves_generate_single(board, moves, pos, knight_dirs, sizeof(knight_dirs) / sizeof(knight_dirs[0]));
            break;
        case PIECE_TYPE_BISHOP:
            moves_generate_sliding(board, moves, pos, diagonal_dirs, sizeof(diagonal_dirs) / sizeof(diagonal_dirs[0]));
            break;
        case PIECE_TYPE_ROOK:
            moves_generate_sliding(board, moves, pos, orthogonal_dirs, sizeof(orthogonal_dirs) / sizeof(orthogonal_dirs[0]));
            break;
        case PIECE_TYPE_QUEEN:
            moves_generate_sliding(board, moves, pos, cardinal_dirs, sizeof(cardinal_dirs) / sizeof(cardinal_dirs[0]));
            break;
        case PIECE_TYPE_KING:
            moves_generate_single(board, moves, pos, cardinal_dirs, sizeof(cardinal_dirs) / sizeof(cardinal_dirs[0]));
            break;
    }
}
