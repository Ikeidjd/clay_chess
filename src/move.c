#include "move.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

Move move_normal_wrap(MoveNormal move) {
    return (Move) {
        .type = MOVE_NORMAL,
        .as.normal = move,
    };
}

Move move_castle_wrap(MoveCastle move) {
    return (Move) {
        .type = MOVE_CASTLE,
        .as.castle = move,
    };
}

Move move_promotion_wrap(MovePromotion move) {
    return (Move) {
        .type = MOVE_PROMOTION,
        .as.promotion = move,
    };
}

Move move_new_empty(void) {
    return (Move) {
        .type = MOVE_EMPTY,
    };
}

Move move_new_normal(Pos from, Pos to) {
    return move_normal_wrap((MoveNormal) {
        .from = from,
        .to = to,
    });
}

Move move_new_castle(MoveNormal king_move, MoveNormal rook_move) {
    return move_castle_wrap((MoveCastle) {
        .king_move = king_move,
        .rook_move = rook_move,
    });
}

Move move_new_promotion(MoveNormal move, Piece transgender) {
    return move_promotion_wrap((MovePromotion) {
        .move = move,
        .transgender = transgender,
    });
}

bool move_is_empty(Move move) {
    return move.type == MOVE_EMPTY;
}

void move_execute(Board* board, Move move) {
    switch (move.type) {
        case MOVE_EMPTY:
            fprintf(stderr, "Empty move cannot be executed");
            exit(-1);
            break;
        case MOVE_NORMAL:
            board_move(board, move.as.normal.from, move.as.normal.to);
            break;
        case MOVE_CASTLE:
            move_execute(board, move_normal_wrap(move.as.castle.king_move));
            move_execute(board, move_normal_wrap(move.as.castle.rook_move));
            break;
        case MOVE_PROMOTION:
            move_execute(board, move_normal_wrap(move.as.promotion.move));
            board_set(board, move.as.promotion.move.to, move.as.promotion.transgender);
            break;
    }
}

static void insert(MoveNotation* name, size_t* index, const char* c_str) {
    for(size_t i = 0; c_str[i] != '\0'; i++) {
        name->data[(*index)++] = c_str[i];
    }
}

static void insert_char(MoveNotation* name, size_t* index, char c) {
    if (c != '\0') name->data[(*index)++] = c;
}

MoveNotation move_get_notation(const Board* board, Move move) {
    MoveNotation name = { 0 };
    size_t index = 0;

    #define INSERT(s) insert(&name, &index, s)
    #define INSERT_CHAR(c) insert_char(&name, &index, c)

    switch (move.type) {
        case MOVE_EMPTY:
            fprintf(stderr, "Empty move has no name");
            exit(-1);
            break;
        case MOVE_NORMAL:
            INSERT_CHAR(piece_get_notation(board_get(board, move.as.normal.from)));
            INSERT(pos_get_notation(move.as.normal.from).data);
            INSERT(pos_get_notation(move.as.normal.to).data);
            break;
        case MOVE_CASTLE:
            INSERT("O-O");
            if (move.as.castle.king_move.from.col < move.as.castle.rook_move.from.col) {
                INSERT("-O");
            }
            break;
        case MOVE_PROMOTION:
            INSERT(move_get_notation(board, move_normal_wrap(move.as.promotion.move)).data);
            INSERT_CHAR('=');
            INSERT_CHAR(piece_get_notation(move.as.promotion.transgender));
            break;
    }

    #undef INSERT
    #undef INSERT_CHAR

    return name;
}
