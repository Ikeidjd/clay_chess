#include "board.h"

#include <stddef.h>
#include <assert.h>

Board board_new_empty(void) {
    return (Board) {
        .pieces = { 0 },
        .en_passant = pos_new_invalid(),
        .can_castle_kingside_white = true,
        .can_castle_queenside_white = true,
        .can_castle_kingside_black = true,
        .can_castle_queenside_black = true,
    };
}

Board board_new_normal(void) {
    Board board = board_new_empty();

    board_set(&board, (Pos) { 0, 0 }, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 1 }, (Piece) { .type = PIECE_TYPE_KNIGHT, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 2 }, (Piece) { .type = PIECE_TYPE_BISHOP, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 3 }, (Piece) { .type = PIECE_TYPE_QUEEN, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 4 }, (Piece) { .type = PIECE_TYPE_KING, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 5 }, (Piece) { .type = PIECE_TYPE_BISHOP, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 6 }, (Piece) { .type = PIECE_TYPE_KNIGHT, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 7 }, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_BLACK });

    board_set(&board, (Pos) { 7, 0 }, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 1 }, (Piece) { .type = PIECE_TYPE_KNIGHT, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 2 }, (Piece) { .type = PIECE_TYPE_BISHOP, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 3 }, (Piece) { .type = PIECE_TYPE_QUEEN, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 4 }, (Piece) { .type = PIECE_TYPE_KING, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 5 }, (Piece) { .type = PIECE_TYPE_BISHOP, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 6 }, (Piece) { .type = PIECE_TYPE_KNIGHT, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 7 }, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_WHITE });

    for (int col = 0; col < BOARD_SIZE; col++) {
        board_set(&board, (Pos) { 1, col }, (Piece) { .type = PIECE_TYPE_PAWN, .color = PIECE_COLOR_BLACK });
        board_set(&board, (Pos) { 6, col }, (Piece) { .type = PIECE_TYPE_PAWN, .color = PIECE_COLOR_WHITE });
    }

    return board;
}

Board board_new_pawnless(void) {
    Board board = board_new_empty();

    board_set(&board, (Pos) { 0, 0 }, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 1 }, (Piece) { .type = PIECE_TYPE_KNIGHT, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 2 }, (Piece) { .type = PIECE_TYPE_BISHOP, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 3 }, (Piece) { .type = PIECE_TYPE_QUEEN, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 4 }, (Piece) { .type = PIECE_TYPE_KING, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 5 }, (Piece) { .type = PIECE_TYPE_BISHOP, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 6 }, (Piece) { .type = PIECE_TYPE_KNIGHT, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 7 }, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_BLACK });

    board_set(&board, (Pos) { 7, 0 }, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 1 }, (Piece) { .type = PIECE_TYPE_KNIGHT, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 2 }, (Piece) { .type = PIECE_TYPE_BISHOP, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 3 }, (Piece) { .type = PIECE_TYPE_QUEEN, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 4 }, (Piece) { .type = PIECE_TYPE_KING, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 5 }, (Piece) { .type = PIECE_TYPE_BISHOP, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 6 }, (Piece) { .type = PIECE_TYPE_KNIGHT, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 7 }, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_WHITE });

    return board;
}

Board board_new_castle(void) {
    Board board = board_new_empty();

    board_set(&board, (Pos) { 0, 0 }, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 4 }, (Piece) { .type = PIECE_TYPE_KING, .color = PIECE_COLOR_BLACK });
    board_set(&board, (Pos) { 0, 7 }, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_BLACK });

    board_set(&board, (Pos) { 7, 0 }, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 4 }, (Piece) { .type = PIECE_TYPE_KING, .color = PIECE_COLOR_WHITE });
    board_set(&board, (Pos) { 7, 7 }, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_WHITE });

    return board;
}

Piece board_get(const Board* board, Pos pos) {
    assert(pos.row < BOARD_SIZE && pos.col < BOARD_SIZE);
    return board->pieces[pos.row][pos.col];
}

void board_set(Board* board, Pos pos, Piece piece) {
    assert(pos.row < BOARD_SIZE && pos.col < BOARD_SIZE);
    board->pieces[pos.row][pos.col] = piece;
}

bool board_is_in_bounds(Pos pos) {
    return pos.row >= 0 && pos.col >= 0 && pos.row < BOARD_SIZE && pos.col < BOARD_SIZE;
}

void board_move(Board* board, Pos from, Pos to) {
    board_set(board, to, board_get(board, from));
    board_set(board, from, piece_new_empty());
}

Piece board_next_piece(const Board* board, Pos* pos) {
    Piece piece = piece_new_empty();

    if (pos_eq(*pos, pos_new_invalid())) {
        pos->col = BOARD_SIZE;
    }

    while (piece_is_empty(piece)) {
        pos->col++;

        if (pos->col >= BOARD_SIZE) {
            pos->col = 0;
            pos->row++;
        }

        if (!board_is_in_bounds(*pos)) break;
        piece = board_get(board, *pos);
    }

    return piece;
}

bool board_can_castle_kingside(const Board* board, PieceColor color) {
    return color == PIECE_COLOR_WHITE ? board->can_castle_kingside_white : board->can_castle_kingside_black;
}

void board_disable_can_castle_kingside(Board* board, PieceColor color) {
    bool* can_castle_kingside = color == PIECE_COLOR_WHITE ? &board->can_castle_kingside_white : &board->can_castle_kingside_black;
    *can_castle_kingside = false;
}

bool board_can_castle_queenside(const Board* board, PieceColor color) {
    return color == PIECE_COLOR_WHITE ? board->can_castle_queenside_white : board->can_castle_queenside_black;
}

void board_disable_can_castle_queenside(Board* board, PieceColor color) {
    bool* can_castle_queenside = color == PIECE_COLOR_WHITE ? &board->can_castle_queenside_white : &board->can_castle_queenside_black;
    *can_castle_queenside = false;
}
