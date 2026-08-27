#include <stdio.h>
#include <stdlib.h>

#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay/renderers/raylib/clay_renderer_raylib.c"

#define RAYMATH_IMPLEMENTATION
#include "clay/renderers/raylib/raymath.h"
#include "raylib.h"

void handleClayErrors(Clay_ErrorData errorData) {
    printf("%d", errorData.errorType);
    printf("%.*s", errorData.errorText.length, errorData.errorText.chars);
    exit(-1);
}

Clay_Arena initClay(size_t screenWidth, size_t screenHeight) {
    uint64_t total_memory_size = Clay_MinMemorySize();
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(total_memory_size, malloc(total_memory_size));
    Clay_Initialize(arena, (Clay_Dimensions) { screenWidth, screenHeight }, (Clay_ErrorHandler) { handleClayErrors });
    return arena;
}

int main() {
    const int screenWidth = 600;
    const int screenHeight = 600;

    Clay_Raylib_Initialize(screenWidth, screenHeight, "Clay Test", FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    Clay_Arena arena = initClay(screenWidth, screenHeight);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        Vector2 mousePos = GetMousePosition();
        Vector2 mouseWheel = GetMouseWheelMoveV();

        Clay_SetLayoutDimensions((Clay_Dimensions) { GetScreenWidth(), GetScreenHeight() });
        Clay_SetPointerState((Clay_Vector2) { mousePos.x, mousePos.y }, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
        Clay_UpdateScrollContainers(true, (Clay_Vector2) { mouseWheel.x, mouseWheel.y }, deltaTime);

        Clay_BeginLayout();

        CLAY(CLAY_ID("panel"), (Clay_ElementDeclaration) {
            .layout = { .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_GROW() } },
            .backgroundColor = { 120, 120, 160, 255 },
        }) {
            
        }

        Clay_RenderCommandArray renderCommands = Clay_EndLayout(deltaTime);

        BeginDrawing();

        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, NULL);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
