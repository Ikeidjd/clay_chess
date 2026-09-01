#include "game_state.h"

#include "clay_renderer_raylib.h"

#include "raylib.h"

#include "fonts.h"

GameState state_main_wrap(MainState state) {
    return (GameState) {
        .type = GAME_STATE_MAIN,
        .as.main = state,
    };
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
    Clay_Raylib_Render(render_commands, fonts);

    EndDrawing();
}
