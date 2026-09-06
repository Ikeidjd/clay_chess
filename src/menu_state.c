#include "game_state.h"

#include <string.h>

#include "util.h"
#include "fonts.h"

static void button(const char* id, const char* text, GameState* state, void(*on_click)(GameState* state)) {
    Clay_String str = {
        .chars = id,
        .length = strlen(id),
        .isStaticallyAllocated = true,
    };

    Clay_String text_str = {
        .chars = text,
        .length = strlen(text),
        .isStaticallyAllocated = true,
    };

    CLAY(CLAY_SID(str), (Clay_ElementDeclaration) {
        .layout = {
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
            .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER },
            .padding = CLAY_PADDING_ALL(scale_with_screen(16)),
        },
        .backgroundColor = { 100, 100, 255, 255 },
    }) {
        if (Clay_Hovered() && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            on_click(state);
        }
        CLAY_TEXT(text_str, (Clay_TextElementConfig) {
            .fontId = FONT_NORMAL,
            .fontSize = scale_with_screen(32),
            .textColor = { 200, 200, 255, 255 },
            .textAlignment = CLAY_TEXT_ALIGN_CENTER,
        });
    }
}

static Board board;

void start_game_button_on_click(GameState* state) {
    MainState next_state = state_main_new(board, PIECE_COLOR_WHITE);
    if (state_main_start_game(&next_state, "localhost", "3940") != SOCKET_INVALID) *state = state_main_wrap(next_state);
}

void join_game_button_on_click(GameState* state) {
    MainState next_state = state_main_new(board, PIECE_COLOR_BLACK);
    if (state_main_join_game(&next_state, "localhost", "3940") != SOCKET_INVALID) *state = state_main_wrap(next_state);
}

void offline_game_button_on_click(GameState* state) {
    *state = state_main_wrap(state_main_new(board, PIECE_COLOR_WHITE));
}

void debug_game_button_on_click(GameState* state) {
    *state = state_main_wrap(state_main_new_debug(board));
}

Clay_RenderCommandArray state_menu_update(float delta_time, GameState* state) {
    board = board_new_normal();

    Clay_BeginLayout();

    CLAY(CLAY_ID("panel"), (Clay_ElementDeclaration) {
        .layout = {
            .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_GROW(0) },
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
            .padding = CLAY_PADDING_ALL(scale_with_screen(16)),
        },
    }) {
        CLAY(CLAY_ID("buttons"), (Clay_ElementDeclaration) {
            .layout = {
                .sizing = { CLAY_SIZING_GROW(0), CLAY_SIZING_FIT(0) },
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .childGap = scale_with_screen(16),
            },
        }) {
            button("start_game_button", "START GAME", state, start_game_button_on_click);
            button("join_game_button", "JOIN GAME", state, join_game_button_on_click);
            button("offline_game_button", "OFFLINE GAME", state, offline_game_button_on_click);
            button("debug_game_button", "DEBUG GAME", state, debug_game_button_on_click);
        }
    }

    #undef BUTTON

    return Clay_EndLayout(delta_time);
}
