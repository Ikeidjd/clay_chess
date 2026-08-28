#include <stdio.h>
#include <stdlib.h>

#define CLAY_IMPLEMENTATION
#include "clay.h"
#include "clay/renderers/raylib/clay_renderer_raylib.c"

#define RAYMATH_IMPLEMENTATION
#include "clay/renderers/raylib/raymath.h"
#include "raylib.h"

#include "pos.h"
#include "piece.h"
#include "board.h"
#include "move.h"
#include "move_generation.h"

Texture2D move_indicator_texture;
Texture2D capture_indicator_texture;

void handle_clay_errors(Clay_ErrorData errorData) {
    printf("%d", errorData.errorType);
    printf("%.*s", errorData.errorText.length, errorData.errorText.chars);
    exit(-1);
}

Clay_RenderCommandArray build_layout(float delta_time, Board* board, Pos* selected_pos, MoveBoard* moves) {
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
                        Pos pos = { row, col };
                        Piece piece = board_get(board, pos);
                        Move move = move_board_get(moves, pos);

                        Clay_Color square_color = pos_eq(*selected_pos, pos)
                            ? (Clay_Color) { 160, 160, 0, 255 }
                            : (row + col) % 2 == 0 ? (Clay_Color) { 200, 200, 200, 255 } : (Clay_Color) { 50, 100, 50, 255 };

                        CLAY(CLAY_IDI("board_square", row * 8 + col), (Clay_ElementDeclaration) {
                            .layout = {
                                .sizing = { CLAY_SIZING_PERCENT(0.125) },
                            },
                            .backgroundColor = square_color,
                            .aspectRatio = 1,
                        }) {
                            
                            CLAY(CLAY_IDI("board_piece", row * 8 + col), (Clay_ElementDeclaration) {
                                .layout = {
                                    .sizing = { CLAY_SIZING_GROW() },
                                },
                                .image = { piece_get_texture(piece) },
                                .aspectRatio = 1,
                            }) {
                                if (!move_is_empty(move_board_get(moves, pos))) {
                                    CLAY(CLAY_IDI("move_indicator", row * 8 + col), (Clay_ElementDeclaration) {
                                        .layout = {
                                            .sizing = { CLAY_SIZING_GROW() }
                                        },
                                        .image = { piece_is_empty(piece) ? &move_indicator_texture : &capture_indicator_texture },
                                        .aspectRatio = 1,
                                    });
                                }
                            }

                            if (Clay_Hovered()) {
                                if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && move_is_empty(move)) {
                                    *selected_pos = piece_is_empty(piece) ? pos_new_invalid() : pos;
                                    moves_generate(board, moves, pos);
                                } else if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && !move_is_empty(move)) {
                                    move_execute(board, move);
                                    *selected_pos = pos_new_invalid();
                                    *moves = (MoveBoard) { 0 };
                                }
                            }
                        }
                    }
                }
            }
        }

        return Clay_EndLayout(delta_time);
}

int main() {
    Board board = board_new_pawnless();
    Pos selected_pos = pos_new_invalid();
    MoveBoard moves = { 0 };

    const int screen_width = 768;
    const int screen_height = 768;

    Clay_Raylib_Initialize(screen_width, screen_height, "Clay Test", FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    piece_textures_init();
    move_indicator_texture = LoadTexture("res/move_indicator.png");
    capture_indicator_texture = LoadTexture("res/capture_indicator.png");

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

        Clay_RenderCommandArray renderCommands = build_layout(delta_time, &board, &selected_pos, &moves);

        BeginDrawing();

        ClearBackground(BLACK);
        Clay_Raylib_Render(renderCommands, NULL);

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
