#include "game_state.h"

#include <stdio.h>

#include "textures.h"

static void detect_piece_mouse_input(MainState* self, Pos pos) {
    if (!Clay_Hovered()) return;

    Piece piece = board_get(&self->board, pos);
    Move move = move_board_get(&self->moves, pos);

    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && move_is_empty(move)) {
        self->selected_pos = piece_is_empty(piece) ? pos_new_invalid() : pos;
        moves_generate(&self->board, &self->moves, pos, self->can_castle_kingside, self->can_castle_queenside);
    } else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !move_is_empty(move)) {
        printf("%s\n", move_get_notation(&self->board, move).data);
        move_execute(&self->board, move);
        self->selected_pos = pos_new_invalid();
        self->moves = (MoveBoard) { 0 };
    }
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
            for (size_t row = 0; row < 8; row++) {
                CLAY(CLAY_IDI("board_row", row), (Clay_ElementDeclaration) {
                    .layout = {
                        .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_PERCENT(0.125) },
                    },
                })
                for (size_t col = 0; col < 8; col++) {
                    build_piece_layout(self, (Pos) { row, col });
                }
            }
        }
    }

    return Clay_EndLayout(delta_time);
}
