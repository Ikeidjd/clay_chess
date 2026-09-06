#include "game_state.h"
#include "main_state_include.c"

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "textures.h"
#include "fonts.h"
#include "socket.h"

MainState state_main_new(Board board, PieceColor my_turn) {
    MainState state = state_main_new_debug(board);
    state.my_turn = my_turn;
    state.cur_turn = PIECE_COLOR_WHITE;
    return state;
}

MainState state_main_new_debug(Board board) {
    return (MainState) {
        .moves = { 0 },
        .board = board,
        .captured_pieces = { 0 },
        .promotion_options = { 0 },
        .promotion = { 0 },
        .selected_pos = pos_new_invalid(),
        .check_pos = pos_new_invalid(),
        .game_over = GAME_OVER_NONE,
        .my_turn = PIECE_COLOR_EMPTY,
        .cur_turn = PIECE_COLOR_EMPTY,
        .host_socket = SOCKET_INVALID,
        .guest_socket = SOCKET_INVALID,
        .is_en_passant_visible = false,
        .should_detect_checks = true,
    };
}

sock_t state_main_start_game(MainState* self, const char* host, const char* port) {
    self->host_socket = socket_start(host, port);
    return self->host_socket;
}

sock_t state_main_join_game(MainState* self, const char* host, const char* port) {
    self->guest_socket = socket_join(host, port);
    return self->guest_socket;
}

Clay_RenderCommandArray state_main_update(MainState* self, float delta_time) {
    if (should_try_to_connect(self)) {
        return try_to_connect(self, delta_time);
    }

    if (!is_online(self)) {
        self->my_turn = self->cur_turn;
    }

    if (!is_my_turn(self)) {
        char buf[MOVE_NOTATION_SIZE] = { 0 };

        if (socket_recv(self->guest_socket, buf, MOVE_NOTATION_SIZE, 0) != SOCK_ERROR) {
            printf("Message received: %s\n", buf);
            Pos pos = pos_new_invalid();

            while (board_next_piece(&self->board, &pos)) {
                if (board_get(&self->board, pos).color == self->my_turn) continue;

                MoveArray moves = moves_generate_array(&self->board, pos, self->should_detect_checks);

                for (size_t i = 0; i < moves.count; i++) {
                    Move move = move_array_get(&moves, i);
                    MoveNotation notation = move_get_notation(&self->board, move);

                    if (strcmp(notation.data, buf) == 0) {
                        perform_move(self, move);
                        break;
                    }
                }
            }
        }
    }

    if (IsKeyReleased(KEY_F1)) {
        self->is_en_passant_visible = !self->is_en_passant_visible;
    }

    Clay_BeginLayout();

    CLAY(CLAY_ID("panel"), (Clay_ElementDeclaration) {
        .layout = {
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = { 120, 120, 160, 255 },
    }) {
        float board_size = fmin(GetScreenWidth(), GetScreenHeight()) * 0.8;

        build_casualties_layout(self, board_size, is_debug(self) ? PIECE_COLOR_WHITE : self->my_turn, CLAY_ATTACH_POINT_LEFT_BOTTOM);

        CLAY(CLAY_ID("board"), (Clay_ElementDeclaration) {
            .layout = {
                // Using CLAY_SIZING_GROW(0) instead of CLAY_SIZING_FIXED(board_size), the end_game component wouldn't look right horizontally on a non-square screen, extending more than it should
                .sizing = { CLAY_SIZING_FIXED(board_size), CLAY_SIZING_FIXED(board_size) },
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            },
        }) {
            for (int unprocessed_row = 0; unprocessed_row < BOARD_SIZE; unprocessed_row++) {
                int row = unprocessed_row;
                if (self->my_turn == PIECE_COLOR_BLACK) row = BOARD_SIZE - row - 1;

                CLAY(CLAY_IDI("board_row", row), (Clay_ElementDeclaration) {
                    .layout = {
                        .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_PERCENT(0.125) },
                    },
                })

                for (int unprocessed_col = 0; unprocessed_col < BOARD_SIZE; unprocessed_col++) {
                    int col = unprocessed_col;
                    if (self->my_turn == PIECE_COLOR_BLACK) col = BOARD_SIZE - col - 1;

                    build_board_square_layout(self, (Pos) { row, col });
                }
            }

            build_game_over_layout(self);
        }

        build_casualties_layout(self, board_size, is_debug(self) ? PIECE_COLOR_BLACK : piece_color_swap(self->my_turn), CLAY_ATTACH_POINT_LEFT_TOP);
    }

    return Clay_EndLayout(delta_time);
}
