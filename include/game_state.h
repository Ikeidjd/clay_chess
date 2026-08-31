#pragma once

#include "clay.h"

#include "pos.h"
#include "board.h"
#include "move_generation.h"

typedef struct {
    MoveBoard moves;
    Board board;
    Pos selected_pos;
    bool can_castle_kingside;
    bool can_castle_queenside;
    int host_socket;
    int guest_socket;
} MainState;

Clay_RenderCommandArray state_main_update(MainState* self, float delta_time);

typedef enum {
    GAME_STATE_MAIN,
} GameStateType;

typedef struct {
    GameStateType type;
    union {
        MainState main;
    } as;
} GameState;

GameState state_main_wrap(MainState state);

GameState state_main_new(Board board);
GameState state_main_new_host(Board board, const char* host, const char* port);
GameState state_main_new_host(Board board, const char* host, const char* port);

Clay_RenderCommandArray state_update(GameState* self, float delta_time);
void state_draw(Clay_RenderCommandArray render_commands);
