#include "game_state.h"

#include <stdio.h>
#include <string.h>

#include "textures.h"
#include "fonts.h"
#include "socket.h"

MainState state_main_new(Board board, PieceColor my_turn) {
    return (MainState) {
        .moves = { 0 },
        .board = board,
        .selected_pos = pos_new_invalid(),
        .my_turn = my_turn,
        .cur_turn = PIECE_COLOR_WHITE,
        .can_castle_kingside = true,
        .can_castle_queenside = true,
        .host_socket = SOCKET_INVALID,
        .guest_socket = SOCKET_INVALID,
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

static void detect_piece_mouse_input(MainState* self, Pos pos) {
    if (self->cur_turn != self->my_turn || !Clay_Hovered()) return;

    Piece piece = board_get(&self->board, pos);
    Move move = move_board_get(&self->moves, pos);

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && move_is_empty(move)) {
        if (piece.color == self->my_turn) {
            self->selected_pos = pos;
            self->moves = moves_generate_board(&self->board, pos, self->can_castle_kingside, self->can_castle_queenside);
        } else {
            self->selected_pos = pos_new_invalid();
            self->moves = move_board_new();
        }
    } else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !move_is_empty(move)) {
        MoveNotation notation = move_get_notation(&self->board, move);
        socket_send(self->guest_socket, notation.data, MOVE_NOTATION_SIZE, 0);
        printf("Message sent: %s\n", notation.data);

        move_execute(&self->board, move);

        self->selected_pos = pos_new_invalid();
        self->moves = move_board_new();

        self->cur_turn = piece_color_swap(self->cur_turn);
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
            .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() },
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

static void build_piece_layout(MainState* self, Pos pos) {
    Piece piece = board_get(&self->board, pos);
    Move move = move_board_get(&self->moves, pos);

    Clay_Color square_color = pos_eq(self->selected_pos, pos)
        ? (Clay_Color) { 160, 160, 0, 255 }
        : (pos.row + pos.col) % 2 == 0
            ? (Clay_Color) { 200, 200, 200, 255 }
            : (Clay_Color) { 50, 100, 50, 255 };

    CLAY(CLAY_IDI("board_square", pos.row * 8 + pos.col), (Clay_ElementDeclaration) {
        .layout = {
            .sizing = { CLAY_SIZING_PERCENT(0.125) },
        },
        .backgroundColor = square_color,
        .aspectRatio = { 1 },
    }) {
        
        CLAY(CLAY_IDI("board_piece", pos.row * 8 + pos.col), (Clay_ElementDeclaration) {
            .layout = {
                .sizing = { CLAY_SIZING_GROW() },
            },
            .image = { piece_get_texture(piece) },
            .aspectRatio = { 1 },
        }) {
            detect_piece_mouse_input(self, pos);

            if (!move_is_empty(move)) {
                CLAY(CLAY_IDI("move_indicator", pos.row * 8 + pos.col), (Clay_ElementDeclaration) {
                    .layout = {
                        .sizing = { CLAY_SIZING_GROW() }
                    },
                    .image = { piece_is_empty(piece) ? &textures.move_indicator : &textures.capture_indicator },
                    .aspectRatio = { 1 },
                });
            }
        }
    }
}

Clay_RenderCommandArray state_main_update(MainState* self, float delta_time) {
    if (self->host_socket != SOCKET_INVALID && self->guest_socket == SOCKET_INVALID) {
        return try_to_connect(self, delta_time);
    }

    if (self->cur_turn != self->my_turn) {
        char buf[MOVE_NOTATION_SIZE] = { 0 };

        if (socket_recv(self->guest_socket, buf, MOVE_NOTATION_SIZE, 0) != SOCK_ERROR) {
            printf("Message received: %s\n", buf);
            self->cur_turn = piece_color_swap(self->cur_turn);

            Pos pos = pos_new_invalid();
            while (!piece_is_empty(board_next_piece(&self->board, &pos))) {
                MoveArray moves = moves_generate(&self->board, pos, self->can_castle_kingside, self->can_castle_queenside);

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

                    build_piece_layout(self, (Pos) { row, col });
                }
            }
        }
    }

    return Clay_EndLayout(delta_time);
}
