#pragma once
#include <cstdint>
#include "Dep/raylib.h"

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

        inline RayLib::Color to_ray() {
            return { r, g, b, 0xFF };
        }
};

namespace Colors {
    inline auto BG = ::Color(0x000022);
    inline auto FG = ::Color(0xFFF9A6);
    inline auto ERROR = ::Color(0xFF6464);
}
