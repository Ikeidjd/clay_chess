#include "game_state.h"

#include "fonts.h"

Clay_RenderCommandArray state_menu_update(float delta_time, GameState* state) {
    Clay_BeginLayout();

    #define SIDE(side_id, button_id, button_text, start_or_join, color) \
        do { \
            CLAY(CLAY_ID(side_id), (Clay_ElementDeclaration) { \
                .layout = { \
                    .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_FIT() }, \
                    .layoutDirection = CLAY_TOP_TO_BOTTOM, \
                    .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }, \
                }, \
            }) { \
                CLAY(CLAY_ID(button_id), (Clay_ElementDeclaration) { \
                    .layout = { \
                        .sizing = { CLAY_SIZING_PERCENT(0.75) }, \
                        .childAlignment = { CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER }, \
                        .padding = 16, \
                    }, \
                    .backgroundColor = { 100, 100, 255, 255 }, \
                    .aspectRatio = 5, \
                }) { \
                    if (Clay_Hovered()) { \
                        MainState next_state = state_main_new(board_new_pawnless(), color); \
                        state_main_ ## start_or_join ## _game(&next_state, "localhost", "3940"); \
                        *state = state_main_wrap(next_state); \
                    } \
                    CLAY_TEXT(CLAY_STRING(button_text), (Clay_TextElementConfig) { \
                        .fontId = FONT_NORMAL, \
                        .fontSize = 32, \
                        .textColor = { 200, 200, 255, 255 }, \
                        .textAlignment = CLAY_TEXT_ALIGN_CENTER, \
                    }); \
                } \
            } \
        } while (false)

    CLAY(CLAY_ID("panel"), (Clay_ElementDeclaration) {
        .layout = {
            .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() },
            .layoutDirection = CLAY_LEFT_TO_RIGHT,
            .childAlignment = { .y = CLAY_ALIGN_Y_CENTER },
        },
    }) {
        SIDE("left_side", "start_game_button", "START GAME", start, PIECE_COLOR_WHITE);
        SIDE("right_side", "join_game_button", "JOIN GAME", join, PIECE_COLOR_BLACK);
    }

    #undef SIDE

    return Clay_EndLayout(delta_time);
}
