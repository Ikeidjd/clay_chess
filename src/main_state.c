#include "game_state.h"
#include "main_state_include.c"

#include <stdio.h>
#include <string.h>

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
        .promotion_options = { 0 },
        .promotion = { 0 },
        .selected_pos = pos_new_invalid(),
        .my_turn = PIECE_COLOR_EMPTY,
        .cur_turn = PIECE_COLOR_EMPTY,
        .host_socket = SOCKET_INVALID,
        .guest_socket = SOCKET_INVALID,
        .can_castle_kingside = true,
        .can_castle_queenside = true,
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
            self->cur_turn = piece_color_swap(self->cur_turn);

            Pos pos = pos_new_invalid();
            while (!piece_is_empty(board_next_piece(&self->board, &pos))) {
                MoveArray moves = moves_generate_array(&self->board, pos, self->can_castle_kingside, self->can_castle_queenside);

                for (size_t i = 0; i < moves.count; i++) {
                    Move move = move_array_get(&moves, i);
                    MoveNotation notation = move_get_notation(&self->board, move);

                    if (strcmp(notation.data, buf) == 0) {
                        move_execute(&self->board, move);
                        break;
                    }
                }
            }
        }
    }

    Clay_BeginLayout();

    CLAY(CLAY_ID("panel"), (Clay_ElementDeclaration) {
        .layout = {
            .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() },
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }
        },
        .backgroundColor = { 120, 120, 160, 255 },
    }) {
        CLAY(CLAY_ID("board"), (Clay_ElementDeclaration) {
            .layout = {
                .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() },
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            },
            .aspectRatio = 1,
        }) {
            for (int unprocessed_row = 0; unprocessed_row < BOARD_SIZE; unprocessed_row++) {
                int row = unprocessed_row;
                if (self->my_turn == PIECE_COLOR_BLACK) row = BOARD_SIZE - row - 1;

                CLAY(CLAY_IDI("board_row", row), (Clay_ElementDeclaration) {
                    .layout = {
                        .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_PERCENT(0.125) },
                    },
                })

                for (int unprocessed_col = 0; unprocessed_col < BOARD_SIZE; unprocessed_col++) {
                    int col = unprocessed_col;
                    if (self->my_turn == PIECE_COLOR_BLACK) col = BOARD_SIZE - col - 1;

                    build_board_square_layout(self, (Pos) { row, col });
                }
            }
        }
    }

    return Clay_EndLayout(delta_time);
}
