// The main_state.c file was getting annoyingly big

#include "game_state.h"

#include <stdio.h>

#include "textures.h"
#include "fonts.h"
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

static void perform_move(MainState* self, Move move) {
    if (is_online(self)) {
        MoveNotation notation = move_get_notation(&self->board, move);
        socket_send(self->guest_socket, notation.data, MOVE_NOTATION_SIZE, 0);
        printf("Message sent: %s\n", notation.data);
    }

    move_execute(&self->board, move);

    self->selected_pos = pos_new_invalid();
    self->moves = move_board_new();

    self->cur_turn = piece_color_swap(self->cur_turn);
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
            self->moves = moves_generate_board(&self->board, pos);
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
            .fontSize = 24,
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
    } else if(self->is_en_passant_visible && pos_eq(self->board.en_passant, pos)) {
        square_color = (Clay_Color) { 160, 0, 0, 255 };
    } else if (pos_eq(self->selected_pos, pos)) {
        square_color = (Clay_Color) { 160, 160, 0, 255 };
    } else {
        square_color = (pos.row + pos.col) % 2 == 0 ? (Clay_Color) { 200, 200, 200, 255 } : (Clay_Color) { 50, 100, 50, 255 };
    }

    CLAY(CLAY_IDI("board_square", pos.row * 8 + pos.col), (Clay_ElementDeclaration) {
        .layout = {
            .sizing = { CLAY_SIZING_PERCENT(0.125), { 0 } },
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
