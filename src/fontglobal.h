#pragma once
#include <memory>
#include "Dep/raylib.h"

class Font {
    public:
        static RayLib::Font the() {
            if (!the_raw) {
                the_raw = std::make_unique<RayLib::Font>(RayLib::LoadFont("ibm.ttf"));
            }
            return *the_raw;
        }
    private:
        static std::unique_ptr<RayLib::Font> the_raw;
};