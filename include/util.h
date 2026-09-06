#pragma once

#include <math.h>

#include "raylib.h"

static inline double scale_with_screen(double d) {
    return d * fmin(GetScreenWidth() / 768.0, GetScreenHeight() / 768.0);
}
