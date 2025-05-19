#pragma once
#include "UI/Container/Container.h"
#include "fontglobal.h"
#include <cstring>

class TextLabel : public Container {
    public:
        const char* text = "Test";
        RayLib::Color color = RayLib::BLACK;
        float font_size = Font::the().baseSize;

        TextLabel(const char* _text) {
            char* new_text = (char*) calloc(strlen(_text) + 1, 1);
            strcpy(new_text, _text);
            text = new_text;
            // this->text = _text;
        }

        Vector2 text_bounds() {
            auto text_size = RayLib::MeasureTextEx(Font::the(), text, font_size, 1);
            return Vector2::from_ray(text_size);
        }

        void draw_self() override {
            Vector2 pos = position->get_global();

            size->set_raw(text_bounds());

            RayLib::DrawTextEx(
                Font::the(),
                text,
                { (float)pos.x, (float)pos.y },
                font_size,
                1,
                color
            );
        }
};
