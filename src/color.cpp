#include "color.h"
namespace Ray {
    #include "raylib.h"
}

Color::Color(uint32_t color) {
    b = color & 0xFF;
    g = color >> 8 & 0xFF;
    r = color >> 16 & 0xFF;
}

Ray::Color Color::to_ray() {
    return { r, g, b, 0xFF };
}



namespace Colors {
    Ray::Color BG = Color(0x000022).to_ray();
    Ray::Color FG = Color(0xfff9a6).to_ray();
}