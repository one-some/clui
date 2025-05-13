#pragma once

#include <cstdarg>

namespace RayLib {
    #include "Dep/raylib_raw.h"

    inline bool IsKeyTyped(int key) {
        return IsKeyPressed(key) || IsKeyPressedRepeat(key);
    }
}