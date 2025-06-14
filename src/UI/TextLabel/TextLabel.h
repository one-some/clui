#pragma once
#include "UI/Container/Container.h"
#include "fontglobal.h"
#include "Claire/String.h"

class TextLabel : public Container {
    public:
        String text;
        RayLib::Color color = RayLib::BLACK;
        float font_size = Font::the().baseSize;

        TextLabel(const char* _text): text(_text) {
            size->strategy_x = SizeStrategy::FORCE;
            size->strategy_y = SizeStrategy::FORCE;
        }

        Vector2 text_bounds() {
            auto text_size = RayLib::MeasureTextEx(Font::the(), text.as_c(), font_size, 1);
            return Vector2::from_ray(text_size);
        }

        void draw_self() override {
            Vector2 pos = position->get_global();

            size->set_raw(text_bounds());

            RayLib::DrawTextEx(
                Font::the(),
                text.as_c(),
                { (float)pos.x, (float)pos.y },
                font_size,
                1,
                color
            );
        }
};
