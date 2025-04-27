#pragma once
#include <cstdint>

namespace Ray {
    #include "raylib.h"
}

class Color {
    public:
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;

        Color(uint32_t color);
        Ray::Color to_ray();
};

namespace Colors {
    extern Ray::Color BG;
    extern Ray::Color FG;
}
