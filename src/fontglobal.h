#pragma once
#include <memory>
namespace Ray {
    #include "raylib.h"
}

class Font {
    public:
        static Ray::Font the() {
            if (!the_raw) {
                the_raw = std::make_unique<Ray::Font>(Ray::LoadFont("ibm.ttf"));
            }
            return *the_raw;
        }
    private:
        static std::unique_ptr<Ray::Font> the_raw;
};