#include "move_generation.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "piece.h"

MoveArray move_array_new(void) {
    return (MoveArray) { 0 };
}

Move move_array_get(const MoveArray* moves, size_t i) {
    assert(i < moves->count);
    return moves->moves[i];
}

void move_array_insert(MoveArray* moves, Move move) {
    assert(moves->count < sizeof(moves->moves) / sizeof(moves->moves[0]));
    moves->moves[moves->count++] = move;
}

MoveBoard move_board_new(void) {
    return (MoveBoard) { 0 };
}

Move move_board_get(const MoveBoard* moves, Pos pos) {
    assert(board_is_in_bounds(pos));
    return moves->moves[pos.row][pos.col];
}

void move_board_set(MoveBoard* moves, Pos pos, Move move) {
    assert(board_is_in_bounds(pos));
    moves->moves[pos.row][pos.col] = move;
}

MoveBoard move_array_to_board(const MoveArray* array) {
    MoveBoard board = move_board_new();

    for (size_t i = 0; i < array->count; i++) {
        Move move = move_array_get(array, i);

        Pos pos;
        switch (move.type) {
            case MOVE_EMPTY:
                fprintf(stderr, "Empty move cannot be added to move_board");
                exit(-1);
                break;
            case MOVE_NORMAL:
                pos = move.as.normal.to;
                break;
            case MOVE_CASTLE:
                pos = move.as.castle.king_move.to;
                break;
            case MOVE_PROMOTION:
                pos = move.as.promotion.move.to;
                break;
        }

        move_board_set(&board, pos, move);
    }

    return board;
}

static const Dir knight_dirs[] = {
    { 2, 1 },
    { 2, -1 },
    { -2, 1 },
    { -2, -1 },
    { 1, 2 },
    { -1, 2 },
    { 1, -2 },
    { -1, -2 },
};

static const Dir diagonal_dirs[] = {
    { 1, 1 },
    { 1, -1 },
    { -1, 1 },
    { -1, -1 },
};

static const Dir orthogonal_dirs[] = {
    { 1, 0 },
    { -1, 0 },
    { 0, 1 },
    { 0, -1 },
};

static const Dir cardinal_dirs[] = {
    { 1, 1 },
    { 1, -1 },
    { -1, 1 },
    { -1, -1 },
    { 1, 0 },
    { -1, 0 },
    { 0, 1 },
    { 0, -1 },
};

static const Dir kingside = { 0, 1 };
static const Dir queenside = { 0, -1 };

static void moves_generate_single(const Board* board, MoveArray* moves, Pos from, const Dir* dirs, size_t dirs_length) {
    Piece piece = board_get(board, from);

    for (size_t i = 0; i < dirs_length; i++) {
        Dir dir = dirs[i];
        Pos to = pos_plus_dir(from, dir);

        if (!board_is_in_bounds(to)) continue;

        Piece enemy = board_get(board, to);

        if (piece.color != enemy.color) {
            move_array_insert(moves, move_new_normal(from, to));
        }
    }
}

static void moves_generate_sliding(const Board* board, MoveArray* moves, Pos from, const Dir* dirs, size_t dirs_length) {
    Piece piece = board_get(board, from);

    for (size_t i = 0; i < dirs_length; i++) {
        Dir dir = dirs[i];
        Pos to = pos_plus_dir(from, dir);

        while (board_is_in_bounds(to) && piece_is_empty(board_get(board, to))) {
            move_array_insert(moves, move_new_normal(from, to));
            to = pos_plus_dir(to, dir);
        }

        if (board_is_in_bounds(to) && piece_is_enemy(piece, board_get(board, to))) {
            move_array_insert(moves, move_new_normal(from, to));
        }
    }
}

static void moves_generate_castle(const Board* board, MoveArray* moves, Pos king_from, Dir dir, bool can_castle) {
    if (!can_castle) return;

    Piece king = board_get(board, king_from);
    Pos rook_from = pos_plus_dir(king_from, dir);

    while (board_is_in_bounds(rook_from) && piece_is_empty(board_get(board, rook_from))) {
        rook_from = pos_plus_dir(rook_from, dir);
    }

    if (!board_is_in_bounds(rook_from)) return;

    Piece rook = board_get(board, rook_from);

    if (rook.type != PIECE_TYPE_ROOK || piece_is_enemy(king, rook)) return;

    Pos rook_to = pos_plus_dir(king_from, dir);
    Pos king_to = pos_plus_dir(rook_to, dir);

    move_array_insert(moves, move_new_castle((MoveNormal) { king_from, king_to }, (MoveNormal) { rook_from, rook_to }));
}

static void maybe_insert_promotion(const Board* board, MoveArray* moves, MoveNormal move) {
    Piece pawn = board_get(board, move.from);
    int promotion_row = pawn.color == PIECE_COLOR_WHITE ? 0 : 7;

    if (move.to.row != promotion_row) {
        move_array_insert(moves, move_normal_wrap(move));
        return;
    }

    move_array_insert(moves, move_new_promotion(move, (Piece) { .type = PIECE_TYPE_KNIGHT, .color = pawn.color }));
    move_array_insert(moves, move_new_promotion(move, (Piece) { .type = PIECE_TYPE_BISHOP, .color = pawn.color }));
    move_array_insert(moves, move_new_promotion(move, (Piece) { .type = PIECE_TYPE_ROOK, .color = pawn.color }));
    move_array_insert(moves, move_new_promotion(move, (Piece) { .type = PIECE_TYPE_QUEEN, .color = pawn.color }));
}

static void moves_generate_pawn(const Board* board, MoveArray* moves, Pos from, Pos en_passant) {
    Piece pawn = board_get(board, from);

    Dir forward = { -1, 0 };
    int initial_row = 6;

    if (pawn.color == PIECE_COLOR_BLACK) {
        forward.row = 1;
        initial_row = 1;
    }

    Pos move_to = pos_plus_dir(from, forward);
    Pos move_to_double = pos_plus_dir(move_to, forward);

    if (board_is_in_bounds(move_to) && piece_is_empty(board_get(board, move_to))) {
        maybe_insert_promotion(board, moves, (MoveNormal) { from, move_to });

        if (from.row == initial_row && board_is_in_bounds(move_to_double) && piece_is_empty(board_get(board, move_to_double))) {
            maybe_insert_promotion(board, moves, (MoveNormal) { from, move_to_double });
        }
    }

    Pos capture_to_left = pos_plus_dir(move_to, (Dir) { 0, -1 });
    Pos capture_to_right = pos_plus_dir(move_to, (Dir) { 0, 1 });

    if (board_is_in_bounds(capture_to_left) && piece_is_enemy(pawn, board_get(board, capture_to_left))) {
        maybe_insert_promotion(board, moves, (MoveNormal) { from, capture_to_left });
    }

    if (board_is_in_bounds(capture_to_right) && piece_is_enemy(pawn, board_get(board, capture_to_right))) {
        maybe_insert_promotion(board, moves, (MoveNormal) { from, capture_to_right });
    }
}

MoveArray moves_generate_array(const Board* board, Pos pos, bool can_castle_kingside, bool can_castle_queenside) {
    MoveArray moves = move_array_new();
    Piece piece = board_get(board, pos);

    switch (piece.type) {
        case PIECE_TYPE_EMPTY:
            break;
        case PIECE_TYPE_PAWN:
            moves_generate_pawn(board, &moves, pos, pos_new_invalid());
            break;
        case PIECE_TYPE_KNIGHT:
            moves_generate_single(board, &moves, pos, knight_dirs, sizeof(knight_dirs) / sizeof(knight_dirs[0]));
            break;
        case PIECE_TYPE_BISHOP:
            moves_generate_sliding(board, &moves, pos, diagonal_dirs, sizeof(diagonal_dirs) / sizeof(diagonal_dirs[0]));
            break;
        case PIECE_TYPE_ROOK:
            moves_generate_sliding(board, &moves, pos, orthogonal_dirs, sizeof(orthogonal_dirs) / sizeof(orthogonal_dirs[0]));
            break;
        case PIECE_TYPE_QUEEN:
            moves_generate_sliding(board, &moves, pos, cardinal_dirs, sizeof(cardinal_dirs) / sizeof(cardinal_dirs[0]));
            break;
        case PIECE_TYPE_KING:
            moves_generate_single(board, &moves, pos, cardinal_dirs, sizeof(cardinal_dirs) / sizeof(cardinal_dirs[0]));
            moves_generate_castle(board, &moves, pos, kingside, can_castle_kingside);
            moves_generate_castle(board, &moves, pos, queenside, can_castle_queenside);
            break;
    }

    return moves;
}

MoveBoard moves_generate_board(const Board* board, Pos pos, bool can_castle_kingside, bool can_castle_queenside) {
    MoveArray array = moves_generate_array(board, pos, can_castle_kingside, can_castle_queenside);
    return move_array_to_board(&array);
}
