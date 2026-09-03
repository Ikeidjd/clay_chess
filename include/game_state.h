#pragma once

#include "clay.h"

#include "socket.h"
#include "pos.h"
#include "piece.h"
#include "board.h"
#include "move_generation.h"

typedef struct GameState GameState;

Clay_RenderCommandArray state_menu_update(float delta_time, GameState* state);

#define PROMOTION_OPTIONS_COUNT 4

typedef struct {
    Pos pos;
    Piece transgender;
} PromotionOption;

typedef struct {
    MoveBoard moves;
    Board board;
    PromotionOption promotion_options[PROMOTION_OPTIONS_COUNT];
    Move promotion;
    Pos selected_pos;
    PieceColor my_turn;
    PieceColor cur_turn;
    sock_t host_socket;
    sock_t guest_socket;
    bool is_en_passant_visible;
} MainState;

MainState state_main_new(Board board, PieceColor my_turn);
MainState state_main_new_debug(Board board);

sock_t state_main_start_game(MainState* self, const char* host, const char* port);
sock_t state_main_join_game(MainState* self, const char* host, const char* port);

Clay_RenderCommandArray state_main_update(MainState* self, float delta_time);

typedef enum {
    GAME_STATE_MENU,
    GAME_STATE_MAIN,
} GameStateType;

typedef struct GameState {
    GameStateType type;
    union {
        MainState main;
    } as;
} GameState;

GameState state_menu_wrap(void);
GameState state_main_wrap(MainState state);

Clay_RenderCommandArray state_update(GameState* self, float delta_time);
void state_draw(Clay_RenderCommandArray render_commands);
