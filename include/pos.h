#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "dir.h"

typedef struct {
    size_t row;
    size_t col;
} Pos;

Pos pos_new_invalid();
Pos pos_plus_dir(Pos pos, Dir dir);

bool pos_is_valid(Pos pos);
bool pos_eq(Pos left, Pos right);
