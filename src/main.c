#include <stdio.h>
#include <stdlib.h>

#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay/renderers/raylib/clay_renderer_raylib.c"

#define RAYMATH_IMPLEMENTATION
#include "raymath.h"
#include "raylib.h"

#include "textures.h"
#include "fonts.h"
#include "socket.h"
#include "pos.h"
#include "piece.h"
#include "board.h"
#include "move.h"
#include "move_generation.h"
#include "game_state.h"

static Texture2D move_indicator_texture;
static Texture2D capture_indicator_texture;

static void handle_clay_errors(Clay_ErrorData errorData) {
    printf("%d", errorData.errorType);
    printf("%.*s", errorData.errorText.length, errorData.errorText.chars);
    exit(-1);
}

int main(int argc, char** argv) {
    if (socket_startup() == -1) {
        return -1;
    }

    MainState main_state = state_main_new(board_new_castle());
    if (argc == 1) {
        if (state_main_start_game(&main_state, "localhost", "3490") == SOCKET_INVALID) {
            return -1;
        }
    } else if (state_main_join_game(&main_state, "localhost", "3490") == SOCKET_INVALID) {
        return -1;
    }

    GameState state = state_main_wrap(main_state);

    const int screen_width = 768;
    const int screen_height = 768;

    Clay_Raylib_Initialize(screen_width, screen_height, "Clay Test", FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);
    
    textures_init();
    fonts_init();

    uint64_t total_memory_size = Clay_MinMemorySize();
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(total_memory_size, malloc(total_memory_size));
    Clay_Initialize(arena, (Clay_Dimensions) { screen_width, screen_height }, (Clay_ErrorHandler) { handle_clay_errors });
    Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();
        Vector2 mouse_pos = GetMousePosition();
        Vector2 mouse_wheel = GetMouseWheelMoveV();

        Clay_SetLayoutDimensions((Clay_Dimensions) { GetScreenWidth(), GetScreenHeight() });
        Clay_SetPointerState((Clay_Vector2) { mouse_pos.x, mouse_pos.y }, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
        Clay_UpdateScrollContainers(true, (Clay_Vector2) { mouse_wheel.x, mouse_wheel.y }, delta_time);

        Clay_RenderCommandArray render_commands = state_update(&state, delta_time);
        state_draw(render_commands);
    }

    CloseWindow();

    return 0;
}
