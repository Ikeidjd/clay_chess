#include <stdio.h>
#include <stdlib.h>

#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay/renderers/raylib/clay_renderer_raylib.c"

#define RAYMATH_IMPLEMENTATION
#include "clay/renderers/raylib/raymath.h"
#include "raylib.h"

#include "piece.h"
#include "board.h"

void handle_clay_errors(Clay_ErrorData errorData) {
    printf("%d", errorData.errorType);
    printf("%.*s", errorData.errorText.length, errorData.errorText.chars);
    exit(-1);
}

int main() {
    Board board = { 0 };

    board_set(&board, 0, 0, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_BLACK });
    board_set(&board, 0, 1, (Piece) { .type = PIECE_TYPE_KNIGHT, .color = PIECE_COLOR_BLACK });
    board_set(&board, 0, 2, (Piece) { .type = PIECE_TYPE_BISHOP, .color = PIECE_COLOR_BLACK });
    board_set(&board, 0, 3, (Piece) { .type = PIECE_TYPE_QUEEN, .color = PIECE_COLOR_BLACK });
    board_set(&board, 0, 4, (Piece) { .type = PIECE_TYPE_KING, .color = PIECE_COLOR_BLACK });
    board_set(&board, 0, 5, (Piece) { .type = PIECE_TYPE_BISHOP, .color = PIECE_COLOR_BLACK });
    board_set(&board, 0, 6, (Piece) { .type = PIECE_TYPE_KNIGHT, .color = PIECE_COLOR_BLACK });
    board_set(&board, 0, 7, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_BLACK });

    board_set(&board, 7, 0, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_WHITE });
    board_set(&board, 7, 1, (Piece) { .type = PIECE_TYPE_KNIGHT, .color = PIECE_COLOR_WHITE });
    board_set(&board, 7, 2, (Piece) { .type = PIECE_TYPE_BISHOP, .color = PIECE_COLOR_WHITE });
    board_set(&board, 7, 3, (Piece) { .type = PIECE_TYPE_QUEEN, .color = PIECE_COLOR_WHITE });
    board_set(&board, 7, 4, (Piece) { .type = PIECE_TYPE_KING, .color = PIECE_COLOR_WHITE });
    board_set(&board, 7, 5, (Piece) { .type = PIECE_TYPE_BISHOP, .color = PIECE_COLOR_WHITE });
    board_set(&board, 7, 6, (Piece) { .type = PIECE_TYPE_KNIGHT, .color = PIECE_COLOR_WHITE });
    board_set(&board, 7, 7, (Piece) { .type = PIECE_TYPE_ROOK, .color = PIECE_COLOR_WHITE });

    for (size_t j = 0; j < BOARD_SIZE; j++) {
        board_set(&board, 1, j, (Piece) { .type = PIECE_TYPE_PAWN, .color = PIECE_COLOR_BLACK });
        board_set(&board, 6, j, (Piece) { .type = PIECE_TYPE_PAWN, .color = PIECE_COLOR_WHITE });
    }

    const int screen_width = 768;
    const int screen_height = 768;

    Clay_Raylib_Initialize(screen_width, screen_height, "Clay Test", FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    piece_textures_init();

    uint64_t total_memory_size = Clay_MinMemorySize();
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(total_memory_size, malloc(total_memory_size));
    Clay_Initialize(arena, (Clay_Dimensions) { screen_width, screen_height }, (Clay_ErrorHandler) { handle_clay_errors });

    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();
        Vector2 mouse_pos = GetMousePosition();
        Vector2 mouse_wheel = GetMouseWheelMoveV();

        Clay_SetLayoutDimensions((Clay_Dimensions) { GetScreenWidth(), GetScreenHeight() });
        Clay_SetPointerState((Clay_Vector2) { mouse_pos.x, mouse_pos.y }, IsMouseButtonDown(MOUSE_BUTTON_LEFT));
        Clay_UpdateScrollContainers(true, (Clay_Vector2) { mouse_wheel.x, mouse_wheel.y }, delta_time);

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
                for (size_t i = 0; i < 8; i++) {
                    CLAY(CLAY_IDI("board_row", i), (Clay_ElementDeclaration) {
                        .layout = {
                            .sizing = { CLAY_SIZING_GROW(), CLAY_SIZING_PERCENT(0.125) },
                        },
                    })
                    for (size_t j = 0; j < 8; j++) {
                        Piece piece = board_get(&board, i, j);

                        CLAY(CLAY_IDI("board_square", i * 8 + j), (Clay_ElementDeclaration) {
                            .layout = {
                                .sizing = { CLAY_SIZING_PERCENT(0.125) },
                            },
                            .backgroundColor = (i + j) % 2 == 0 ? (Clay_Color) { 200, 200, 200, 255 } : (Clay_Color) { 50, 100, 50, 255 },
                            .aspectRatio = 1,
                        }) {
                            if (Clay_Hovered() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                                printf("%zu, %zu\n", i, j);
                            }

                            CLAY(CLAY_IDI("board_piece", i * 8 + j), (Clay_ElementDeclaration) {
                                .layout = {
                                    .sizing = { CLAY_SIZING_GROW() },
                                },
                                .image = { piece_get_texture(piece) },
                                .aspectRatio = 1,
                            });
                        }
                    }
                }
            }
        }

        Clay_RenderCommandArray renderCommands = Clay_EndLayout(delta_time);

        BeginDrawing();

        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, NULL);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
