#pragma once
#include <cstdint>
#include "raylib.h"

class Color {
    public:
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;

        Color(uint32_t color);
        RayLib::Color to_ray();
};

namespace Colors {
    extern RayLib::Color BG;
    extern RayLib::Color FG;
}
