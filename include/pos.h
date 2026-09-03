#pragma once

#include <stdbool.h>

#include "dir.h"

typedef struct {
    int row;
    int col;
} Pos;

Pos pos_new_invalid(void);

Pos pos_plus_dir(Pos pos, Dir dir);
Pos pos_between(Pos left, Pos right);

bool pos_is_valid(Pos pos);
bool pos_eq(Pos left, Pos right);

#define POS_NOTATION_SIZE 2

typedef struct {
    char data[POS_NOTATION_SIZE + 1];
} PosNotation;

PosNotation pos_get_notation(Pos pos);
