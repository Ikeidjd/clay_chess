#pragma once

#include <stdint.h>

#include "piece.h"

#define BOARD_SIZE 8

typedef struct {
    Piece pieces[BOARD_SIZE][BOARD_SIZE];
} Board;

Piece board_get(Board* board, size_t i, size_t j);
void board_set(Board* board, size_t i, size_t j, Piece piece);
