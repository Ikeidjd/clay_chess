// The main_state.c file was getting annoyingly big

#include "game_state.h"

#include <stdio.h>
#include <string.h>

#include "util.h"
#include "textures.h"
#include "fonts.h"
#include "sounds.h"
#include "socket.h"

static bool should_try_to_connect(MainState* self) {
    return self->host_socket != SOCKET_INVALID && self->guest_socket == SOCKET_INVALID;
}

static bool is_online(MainState* self) {
    return self->host_socket != SOCKET_INVALID || self->guest_socket != SOCKET_INVALID;
}

static bool is_debug(MainState* self) {
    return self->cur_turn == PIECE_COLOR_EMPTY || self->my_turn == PIECE_COLOR_EMPTY;
}

static bool is_my_turn(MainState* self) {
    return self->cur_turn == self->my_turn || is_debug(self);
}

static bool is_in_promotion(MainState* self) {
    return self->promotion.type == MOVE_PROMOTION;
}

static bool is_selectable(MainState* self, PieceColor color) {
    return color != PIECE_COLOR_EMPTY && (color == self->my_turn || is_debug(self));
}

static void check_capture(MainState* self, Move move) {
    Piece captured_piece = piece_new_empty();

    switch (move.type) {
        case MOVE_EMPTY:
        case MOVE_PAWN_DOUBLE:
        case MOVE_CASTLE:
            break;
        case MOVE_NORMAL:
        case MOVE_PROMOTION:
            captured_piece = board_get(&self->board, move.as.normal.to);
            break;
        case MOVE_EN_PASSANT:
            captured_piece = board_get(&self->board, pos_plus_dir(move.as.en_passant.to, (Dir) { -1, 0 }));
            if (piece_is_empty(captured_piece)) captured_piece = board_get(&self->board, pos_plus_dir(move.as.en_passant.to, (Dir) { 1, 0 }));
            break;
    }

    if (piece_is_empty(captured_piece)) return;

    size_t* captured_pieces_counts = captured_piece.color == PIECE_COLOR_WHITE ? self->captured_pieces.white : self->captured_pieces.black;
    captured_pieces_counts[captured_piece.type]++;
}

static Sound get_move_sound_effect(MainState* self, Move move) {
    switch (move.type) {
        case MOVE_EMPTY:
            fprintf(stderr, "Empty move has no sound associated\n");
            exit(-1);
            break;
        case MOVE_NORMAL:
        case MOVE_PAWN_DOUBLE:
        case MOVE_PROMOTION:
            return piece_is_empty(board_get(&self->board, move.as.normal.to)) ? sounds.move : sounds.capture;
        case MOVE_EN_PASSANT:
            return sounds.capture;
        case MOVE_CASTLE:
            return sounds.castle;
    }
}

static void check_check(MainState* self) {
    self->game_over = is_debug(self) ? GAME_OVER_NONE : GAME_OVER_STALEMATE;
    Pos pos = pos_new_invalid();

    while (board_next_piece(&self->board, &pos)) {
        if (board_get(&self->board, pos).color != self->cur_turn) continue;

        MoveArray moves = moves_generate_array(&self->board, pos, self->should_detect_checks);

        if (moves.count > 0) {
            self->game_over = GAME_OVER_NONE;
            break;
        }
    }

    if (move_is_legal(self->board, move_new_empty(), self->cur_turn)) {
        self->check_pos = pos_new_invalid();
        return;
    }

    self->check_pos = board_get_king(&self->board, self->cur_turn);
    PlaySound(sounds.check);

    if (self->game_over != GAME_OVER_NONE) {
        self->game_over = self->cur_turn == PIECE_COLOR_WHITE ? GAME_OVER_CHECKMATE_BLACK : GAME_OVER_CHECKMATE_WHITE;
    }
}

static void perform_move(MainState* self, Move move) {
    if (is_online(self) && is_my_turn(self)) {
        MoveNotation notation = move_get_notation(&self->board, move);
        socket_send(self->guest_socket, notation.data, MOVE_NOTATION_SIZE, 0);
        printf("Message sent: %s\n", notation.data);
    }

    // Important to put these two statements before move_execute, otherwise, they can't tell if it's a capture
    check_capture(self, move);
    Sound sound = get_move_sound_effect(self, move);

    move_execute(&self->board, move);

    self->selected_pos = pos_new_invalid();
    self->moves = move_board_new();

    self->cur_turn = piece_color_swap(self->cur_turn);

    check_check(self);

    // Only play the move sound effect if there isn't already a check sound effect
    if (!pos_is_valid(self->check_pos)) {
        PlaySound(sound);
    }
}

static void begin_promotion(MainState* self, MovePromotion move) {
    self->selected_pos = pos_new_invalid();
    self->moves = move_board_new();
    self->promotion = move_promotion_wrap(move);

    Piece piece = board_get(&self->board, move.move.from);
    int dir_row = piece.color == PIECE_COLOR_WHITE ? 1 : -1;

    #define INSERT_PROMOTION_OPTION(index, piece_type) \
    do { \
        self->promotion_options[index] = (PromotionOption) { \
            .pos = { move.move.to.row + dir_row * index, move.move.to.col }, \
            .transgender = { .type = piece_type, .color = piece.color }, \
        }; \
    } while (0)
    
    INSERT_PROMOTION_OPTION(0, PIECE_TYPE_QUEEN);
    INSERT_PROMOTION_OPTION(1, PIECE_TYPE_ROOK);
    INSERT_PROMOTION_OPTION(2, PIECE_TYPE_BISHOP);
    INSERT_PROMOTION_OPTION(3, PIECE_TYPE_KNIGHT);

    #undef INSERT_PROMOTION_OPTION
}

static void end_promotion(MainState* self, PromotionOption option) {
    self->promotion.as.promotion.transgender = option.transgender;
    perform_move(self, self->promotion);
    self->promotion = (Move) { 0 };
}

static void detect_piece_mouse_input(MainState* self, Pos pos) {
    if (!is_my_turn(self) || !Clay_Hovered()) return;

    Piece piece = board_get(&self->board, pos);
    Move move = move_board_get(&self->moves, pos);

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && move_is_empty(move)) {
        if (is_selectable(self, piece.color)) {
            self->selected_pos = pos;
            self->moves = moves_generate_board(&self->board, pos, self->should_detect_checks);
        } else {
            self->selected_pos = pos_new_invalid();
            self->moves = move_board_new();
        }
    } else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !move_is_empty(move)) {
        if (move.type == MOVE_PROMOTION) {
            begin_promotion(self, move.as.promotion);
            return;
        }
        
        perform_move(self, move);
    }
}

static Clay_RenderCommandArray try_to_connect(MainState* self, float delta_time) {
    self->guest_socket = socket_try_to_accept(self->host_socket);

    if (socket_would_block(self->guest_socket)) {
        self->guest_socket = SOCKET_INVALID;
    }

    Clay_BeginLayout();

    CLAY(CLAY_ID("trying to connect"), (Clay_ElementDeclaration) {
        .layout = {
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
        },
    }) {
        CLAY_TEXT(CLAY_STRING("Waiting for player 2 to join..."), (Clay_TextElementConfig) {
            .fontId = FONT_NORMAL,
            .fontSize = scale_with_screen(24),
            .textColor = { 255, 255, 255, 255 },
        });
    }

    return Clay_EndLayout(delta_time);
}

static void build_promotion_option_layout(MainState* self, PromotionOption option) {
    CLAY(CLAY_IDI("board_piece", option.pos.row * 8 + option.pos.col), (Clay_ElementDeclaration) {
        .layout = {
            .sizing = { CLAY_SIZING_GROW(0), { 0 } },
        },
        .image = { piece_get_texture(option.transgender) },
        .aspectRatio = { 1 },
    }) {
        if (Clay_Hovered() && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            end_promotion(self, option);
        }
    }
}

static void build_piece_layout(MainState* self, Pos pos) {
    Piece piece = board_get(&self->board, pos);
    Move move = move_board_get(&self->moves, pos);

    CLAY(CLAY_IDI("board_piece", pos.row * 8 + pos.col), (Clay_ElementDeclaration) {
        .layout = {
            .sizing = { CLAY_SIZING_GROW(0), { 0 } },
        },
        .image = { piece_get_texture(piece) },
        .aspectRatio = { 1 },
    }) {
        if (!is_in_promotion(self)) {
            detect_piece_mouse_input(self, pos);
        }

        if (!move_is_empty(move)) {
            CLAY(CLAY_IDI("move_indicator", pos.row * 8 + pos.col), (Clay_ElementDeclaration) {
                .layout = {
                    .sizing = { CLAY_SIZING_GROW(0), { 0 } }
                },
                .image = { piece_is_empty(piece) ? &textures.move_indicator : &textures.capture_indicator },
                .aspectRatio = { 1 },
            });
        }
    }
}

static void build_board_square_layout(MainState* self, Pos pos) {
    PromotionOption* optional_option = NULL;

    if (is_in_promotion(self)) {
        for (size_t i = 0; i < PROMOTION_OPTIONS_COUNT; i++) {
            PromotionOption* option = &self->promotion_options[i];

            if (pos_eq(pos, option->pos)) {
                optional_option = option;
                break;
            }
        }
    }

    Clay_Color square_color;

    if (optional_option != NULL) {
        square_color = (Clay_Color) { 255, 255, 255, 255 };
    } else if (pos_eq(self->check_pos, pos) || self->is_en_passant_visible && pos_eq(self->board.en_passant, pos)) {
        square_color = (Clay_Color) { 160, 0, 0, 255 };
    } else if (pos_eq(self->selected_pos, pos)) {
        square_color = (Clay_Color) { 160, 160, 0, 255 };
    } else {
        square_color = (pos.row + pos.col) % 2 == 0 ? (Clay_Color) { 200, 200, 200, 255 } : (Clay_Color) { 50, 100, 50, 255 };
    }

    CLAY(CLAY_IDI("board_square", pos.row * 8 + pos.col), (Clay_ElementDeclaration) {
        .layout = {
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
        },
        .backgroundColor = square_color,
        .aspectRatio = { 1 },
    }) {
        if (optional_option == NULL) {
            build_piece_layout(self, pos);
        } else {
            build_promotion_option_layout(self, *optional_option);
        }
    }
}

static Clay_String get_game_over_message(MainState* self) {
    switch (self->game_over) {
        case GAME_OVER_NONE:
            return CLAY_STRING("");
        case GAME_OVER_STALEMATE:
            return CLAY_STRING("STALEMATE");
        case GAME_OVER_CHECKMATE_WHITE:
            return CLAY_STRING("WHITE CHECKMATED BLACK");
        case GAME_OVER_CHECKMATE_BLACK:
            return CLAY_STRING("BLACK CHECKMATED WHITE");
    }
}

static void build_game_over_layout(MainState* self) {
    if (self->game_over == GAME_OVER_NONE) return;

    CLAY(CLAY_ID("game_over"), (Clay_ElementDeclaration) {
        .layout = {
            .sizing = { CLAY_SIZING_PERCENT(0.875), CLAY_SIZING_PERCENT(0.625) },
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
        },
        .floating = {
            .attachTo = CLAY_ATTACH_TO_PARENT,
            .attachPoints = {
                .element = CLAY_ATTACH_POINT_CENTER_CENTER,
                .parent = CLAY_ATTACH_POINT_CENTER_CENTER,
            }
        },
        .backgroundColor = { 184, 111, 80, 222 },
        .cornerRadius = CLAY_CORNER_RADIUS(scale_with_screen(64)),
        .border = {
            .color = { 144, 77, 60, 255 },
            .width = CLAY_BORDER_ALL(scale_with_screen(8)),
        },
    }) {
        CLAY(CLAY_ID("game_over_reason"), (Clay_ElementDeclaration) {
            .layout = {
                .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
                .childGap = scale_with_screen(16),
            },
        }) {
            CLAY_TEXT(CLAY_STRING("GAME OVER"), (Clay_TextElementConfig) {
                .fontId = FONT_NORMAL,
                .fontSize = scale_with_screen(72),
                .textAlignment = CLAY_TEXT_ALIGN_CENTER,
                .textColor = { 255, 255, 255, 255 },
            });

            CLAY_TEXT(get_game_over_message(self), (Clay_TextElementConfig) {
                .fontId = FONT_NORMAL,
                .fontSize = scale_with_screen(48),
                .textAlignment = CLAY_TEXT_ALIGN_CENTER,
                .textColor = { 200, 200, 200, 255 },
            });
        }

        CLAY(CLAY_ID("game_over_options"), (Clay_ElementDeclaration) {
            .layout = {
                .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .padding = CLAY_PADDING_ALL(scale_with_screen(32)),
                .childGap = scale_with_screen(32),
            },
        }) {
            Clay_ElementDeclaration side = {
                .layout = {
                    .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
                    .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
                },
            };

            Clay_ElementDeclaration button = {
                .layout = {
                    .sizing = { CLAY_SIZING_FIT(0), CLAY_SIZING_FIT(0) },
                    .padding = CLAY_PADDING_ALL(scale_with_screen(16)),
                },
                .backgroundColor = { 255, 100, 80, 222 },
                .border = {
                    .color = { 200, 66, 60, 255 },
                    .width = CLAY_BORDER_ALL(scale_with_screen(4)),
                },
                .cornerRadius = CLAY_CORNER_RADIUS(scale_with_screen(16)),
            };

            Clay_TextElementConfig text = {
                .fontId = FONT_NORMAL,
                .fontSize = scale_with_screen(36),
                .textAlignment = CLAY_TEXT_ALIGN_CENTER,
                .textColor = { 0, 0, 0, 255 },
            };

            CLAY(CLAY_ID("game_over_options_left"), side) {
                CLAY(CLAY_ID("main_menu_button"), button) {
                    CLAY_TEXT(CLAY_STRING("MAIN MENU"), text);
                }
            }

            CLAY(CLAY_ID("game_over_options_right"), side) {
                CLAY(CLAY_ID("rematch_button"), button) {
                    // With the spaces on the sides, it has the same length as MAIN MENU, so it looks even
                    // (non-monospace fonts don't exist, trust)
                    CLAY_TEXT(CLAY_STRING(" REMATCH "), text);
                }
            }
        }
    }
}

static void build_casualties_layout(MainState* self, float board_size, PieceColor color, Clay_FloatingAttachPointType attachment) {
    const char* id = color == PIECE_COLOR_WHITE ? "white_casualties" : "black_casualties";
    const char* inner_id = color == PIECE_COLOR_WHITE ? "white_casualty" : "black_casualty";

    size_t* captured_pieces_counts = color == PIECE_COLOR_WHITE ? self->captured_pieces.white : self->captured_pieces.black;

    Clay_String str = {
        .chars = id,
        .length = strlen(id),
        .isStaticallyAllocated = true,
    };

    Clay_String inner_str = {
        .chars = inner_id,
        .length = strlen(inner_id),
        .isStaticallyAllocated = true,
    };

    CLAY(CLAY_SID(str), (Clay_ElementDeclaration) {
        .layout = {
            .sizing = { CLAY_SIZING_FIXED(board_size), CLAY_SIZING_PERCENT(0.1) },
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .childAlignment = { CLAY_ALIGN_X_LEFT, CLAY_ALIGN_Y_BOTTOM },
        },
    }) {
        float offset = 0;
        float size = 0.0625 * board_size;

        for (PieceType piece_type = PIECE_TYPE_PAWN; piece_type <= PIECE_TYPE_KING; piece_type++) {
            size_t count = captured_pieces_counts[piece_type];

            for (size_t i = 0; i < count; i++) {
                CLAY(CLAY_SIDI(inner_str, piece_type * 64 + i), (Clay_ElementDeclaration) {
                    .layout = {
                        .sizing = { CLAY_SIZING_FIXED(size), CLAY_SIZING_FIXED(size) },
                    },
                    .floating = {
                        .attachTo = CLAY_ATTACH_TO_PARENT,
                        .attachPoints = {
                            .element = attachment,
                            .parent = attachment,
                        },
                        .offset = { offset, 0 },
                    },
                    .image = { piece_get_texture((Piece) { .type = piece_type, .color = color }) },
                });

                offset += size * 0.5;
            }
        }
    }
}
