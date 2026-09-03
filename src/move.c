#include "move.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "move_generation.h"

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

Move move_pawn_double_wrap(MoveNormal move) {
    return (Move) {
        .type = MOVE_PAWN_DOUBLE,
        .as.pawn_double = move,
    };
}

Move move_en_passant_wrap(MoveNormal move) {
    return (Move) {
        .type = MOVE_EN_PASSANT,
        .as.en_passant = move,
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

Move move_new_pawn_double(Pos from, Pos to) {
    return move_pawn_double_wrap((MoveNormal) {
        .from = from,
        .to = to,
    });
}

Move move_new_en_passant(Pos from, Pos to) {
    return move_en_passant_wrap((MoveNormal) {
        .from = from,
        .to = to,
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

bool move_is_legal(Board board, Move move, PieceColor king_color) {
    move_execute(&board, move);

    Pos king_pos = board_get_king(&board, king_color);
    Pos enemy_pos = pos_new_invalid();

    while (board_next_piece(&board, &enemy_pos)) {
        Piece piece = board_get(&board, enemy_pos);
        if (piece.color == king_color) continue;

        MoveArray moves = moves_generate_array(&board, enemy_pos, false);

        for (size_t i = 0; i < moves.count; i++) {
            Move enemy_move = move_array_get(&moves, i);

            switch (enemy_move.type) {
                case MOVE_NORMAL:
                    if (pos_eq(enemy_move.as.normal.to, king_pos)) return false;
                    break;
                case MOVE_PROMOTION:
                    if (pos_eq(enemy_move.as.promotion.move.to, king_pos)) return false;
                    break;
                case MOVE_EMPTY:
                case MOVE_CASTLE:
                case MOVE_PAWN_DOUBLE:
                case MOVE_EN_PASSANT:
                    break;
            }
        }
    }

    return true;
}

void move_execute(Board* board, Move move) {
    board->en_passant = pos_new_invalid();

    switch (move.type) {
        case MOVE_EMPTY:
            fprintf(stderr, "Empty move cannot be executed");
            exit(-1);
            break;
        case MOVE_NORMAL: {
            Piece piece = board_get(board, move.as.normal.from);

            switch (piece.type) {
                case PIECE_TYPE_KING:
                    board_disable_can_castle_kingside(board, piece.color);
                    board_disable_can_castle_queenside(board, piece.color);
                    break;
                case PIECE_TYPE_ROOK: {
                    Pos queenside_rook_starting_position = { 7, 0 };
                    Pos kingside_rook_starting_position = { 7, 7 };

                    if (piece.color == PIECE_COLOR_BLACK) {
                        queenside_rook_starting_position.row = 0;
                        kingside_rook_starting_position.row = 0;
                    }

                    if (pos_eq(move.as.normal.from, kingside_rook_starting_position) || pos_eq(move.as.normal.to, kingside_rook_starting_position)) {
                        board_disable_can_castle_kingside(board, piece.color);
                    } else if (pos_eq(move.as.normal.from, queenside_rook_starting_position) || pos_eq(move.as.normal.to, queenside_rook_starting_position)) {
                        board_disable_can_castle_queenside(board, piece.color);
                    }

                    break;
                }
                default:
                    break;
            }

            board_move(board, move.as.normal.from, move.as.normal.to);
            break;
        }
        case MOVE_CASTLE:
            move_execute(board, move_normal_wrap(move.as.castle.king_move));
            move_execute(board, move_normal_wrap(move.as.castle.rook_move));
            break;
        case MOVE_PAWN_DOUBLE:
            move_execute(board, move_normal_wrap(move.as.pawn_double));
            board->en_passant.row = (move.as.pawn_double.from.row + move.as.pawn_double.to.row) / 2;
            board->en_passant.col = (move.as.pawn_double.from.col + move.as.pawn_double.to.col) / 2;
            break;
        case MOVE_EN_PASSANT:
            move_execute(board, move_normal_wrap(move.as.en_passant));
            board_set(board, pos_plus_dir(move.as.en_passant.to, (Dir) { -1, 0 }), piece_new_empty());
            board_set(board, pos_plus_dir(move.as.en_passant.to, (Dir) { 1, 0 }), piece_new_empty());
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
        case MOVE_PAWN_DOUBLE:
        case MOVE_EN_PASSANT:
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
