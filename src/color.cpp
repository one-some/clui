#include "Dep/raylib.h"
#include "color.h"

Color::Color(uint32_t color) {
    b = color & 0xFF;
    g = color >> 8 & 0xFF;
    r = color >> 16 & 0xFF;
}

RayLib::Color Color::to_ray() {
    return { r, g, b, 0xFF };
}



namespace Colors {
    RayLib::Color BG = Color(0x000022).to_ray();
    RayLib::Color FG = Color(0xfff9a6).to_ray();
}