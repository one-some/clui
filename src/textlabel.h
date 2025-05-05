#pragma once
#include "UI/Container/Container.h"
#include "fontglobal.h"

class TextLabel : public Container {
    public:
        const char* text = "Test";
        RayLib::Color color = RayLib::BLACK;
        float font_size = Font::the().baseSize;

        TextLabel(const char* _text) {
            this->text = _text;
        }

        void draw_self() override {
            Vector2 pos = position->get_global();

            auto text_size = RayLib::MeasureTextEx(Font::the(), text, font_size, 1);
            size->set_raw(Vector2::from_ray(text_size));

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
