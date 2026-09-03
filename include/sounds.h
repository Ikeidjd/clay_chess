#pragma once

#include "raylib.h"

extern struct Sounds {
    Sound move;
    Sound capture;
    Sound castle;
    Sound check;
} sounds;

void sounds_init(void);
void sounds_destroy(void);
