#include "move.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

Move move_normal_as_move(MoveNormal move) {
    return (Move) {
        .type = MOVE_NORMAL,
        .as.normal = move,
    };
}

Move move_castle_as_move(MoveCastle move) {
    return (Move) {
        .type = MOVE_CASTLE,
        .as.castle = move,
    };
}

Move move_promotion_as_move(MovePromotion move) {
    return (Move) {
        .type = MOVE_PROMOTION,
        .as.promotion = move,
    };
}

Move move_new_empty() {
    return (Move) {
        .type = MOVE_EMPTY,
    };
}

Move move_new_normal(Pos from, Pos to) {
    return (Move) {
        .type = MOVE_NORMAL,
        .as.normal = {
            .from = from,
            .to = to,
        },
    };
}

Move move_new_castle(MoveNormal king_move, MoveNormal rook_move) {
    return (Move) {
        .type = MOVE_CASTLE,
        .as.castle = {
            .king_move = king_move,
            .rook_move = rook_move,
        },
    };
}

Move move_new_promotion(MoveNormal move, Piece transgender) {
    return (Move) {
        .type = MOVE_PROMOTION,
        .as.promotion = {
            .move = move,
            .transgender = transgender,
        },
    };
}

bool move_is_empty(Move move) {
    return move.type == MOVE_EMPTY;
}

void move_execute(Board* board, Move move) {
    switch (move.type) {
        case MOVE_EMPTY:
            printf("Empty move cannot be executed");
            exit(-1);
            break;
        case MOVE_NORMAL:
            board_move(board, move.as.normal.from, move.as.normal.to);
            break;
        case MOVE_CASTLE:
            move_execute(board, move_normal_as_move(move.as.castle.king_move));
            move_execute(board, move_normal_as_move(move.as.castle.rook_move));
            break;
        case MOVE_PROMOTION:
            move_execute(board, move_normal_as_move(move.as.promotion.move));
            board_set(board, move.as.promotion.move.to, move.as.promotion.transgender);
            break;
    }
}
