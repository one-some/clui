#pragma once
#include <memory>
#include "Dep/raylib.h"

struct RLFontDeleter {
    void operator()(RayLib::Font* font_ptr) {
        if (!font_ptr) return;
        RayLib::UnloadFont(*font_ptr);
        delete font_ptr;
    }
};

class Font {
    public:
        static RayLib::Font the() {
            if (!the_raw) {
                // Copy to heap and reset ptr to preserve deleter
                the_raw.reset(new RayLib::Font(RayLib::LoadFont("ibm.ttf")));
            }
            return *the_raw;
        }

    private:
        static std::unique_ptr<RayLib::Font, RLFontDeleter> the_raw;
};