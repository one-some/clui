#pragma once
#include "container.h"
#include "fontglobal.h"

class TextLabel : public Container {
    public:
        const char* text = "Test";
        Ray::Color color = Ray::BLACK;
        float font_size = Font::the().baseSize;

        TextLabel(const char* _text) {
            this->text = _text;
        }

        void draw_self() override {
            Vector2 pos = position->get_global();

            Ray::DrawTextEx(
                Font::the(),
                text,
                { (float)pos.x, (float)pos.y },
                font_size,
                1,
                color
            );
        }
};
