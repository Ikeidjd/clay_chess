#include "game_state.h"

#include "clay_renderer_raylib.h"

#include "raylib.h"

GameState state_main_wrap(MainState state) {
    return (GameState) {
        .type = GAME_STATE_MAIN,
        .as.main = state,
    };
}

GameState state_main_new(Board board) {
    return state_main_wrap((MainState) {
        .moves = { 0 },
        .board = board,
        .selected_pos = pos_new_invalid(),
        .can_castle_kingside = true,
        .can_castle_queenside = true,
    });
}

Clay_RenderCommandArray state_update(GameState* self, float delta_time) {
    switch (self->type) {
        case GAME_STATE_MAIN:
            return state_main_update(&self->as.main, delta_time);
    }
}

void state_draw(Clay_RenderCommandArray render_commands) {
    BeginDrawing();

    ClearBackground(BLACK);
    Clay_Raylib_Render(render_commands, NULL);

    EndDrawing();
}
