#include "game_state.h"

#include "fonts.h"

Clay_RenderCommandArray state_menu_update(float delta_time, GameState* state) {
    Clay_BeginLayout();

    #define BUTTON(button_id, button_text, start_or_join, color, is_debug) \
    do { \
        CLAY(CLAY_ID(button_id), (Clay_ElementDeclaration) { \
            .layout = { \
                .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() }, \
                .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }, \
                .padding = CLAY_PADDING_ALL(16), \
            }, \
            .backgroundColor = { 100, 100, 255, 255 }, \
        }) { \
            if (Clay_Hovered() && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) { \
                if (color == PIECE_COLOR_EMPTY) { \
                    *state = state_main_wrap(is_debug ? state_main_new_debug(board_new_normal()) : state_main_new(board_new_normal(), PIECE_COLOR_WHITE)); \
                } else { \
                    MainState next_state = state_main_new(board_new_normal(), color); \
                    if (state_main_ ## start_or_join ## _game(&next_state, "localhost", "3940") != SOCKET_INVALID) *state = state_main_wrap(next_state); \
                } \
            } \
            CLAY_TEXT(CLAY_STRING(button_text), (Clay_TextElementConfig) { \
                .fontId = FONT_NORMAL, \
                .fontSize = 32, \
                .textColor = { 200, 200, 255, 255 }, \
                .textAlignment = CLAY_TEXT_ALIGN_CENTER, \
            }); \
        } \
    } while(0)

    CLAY(CLAY_ID("panel"), (Clay_ElementDeclaration) {
        .layout = {
            .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() },
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
            .padding = CLAY_PADDING_ALL(16),
        },
    }) {
        CLAY(CLAY_ID("buttons"), (Clay_ElementDeclaration) {
            .layout = {
                .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIT() },
                .layoutDirection = CLAY_LEFT_TO_RIGHT,
                .childGap = 16,
            },
        }) {
            BUTTON("start_game_button", "START GAME", start, PIECE_COLOR_WHITE, false);
            BUTTON("join_game_button", "JOIN GAME", join, PIECE_COLOR_BLACK, false);
            BUTTON("offline_game_button", "OFFLINE GAME", start, PIECE_COLOR_EMPTY, false);
            BUTTON("debug_game_button", "DEBUG GAME", start, PIECE_COLOR_EMPTY, true);
        }
    }

    #undef SIDE

    return Clay_EndLayout(delta_time);
}
