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

        Color(uint32_t color) {
            b = color & 0xFF;
            g = color >> 8 & 0xFF;
            r = color >> 16 & 0xFF;
        }

        Ray::Color to_ray() {
            return { r, g, b, 0xFF };
        }
};

namespace Colors {
    auto BG = Color(0x0000AA);
}
