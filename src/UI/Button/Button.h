#pragma once
#include <functional>
#include "UI/Container/Container.h"
#include "Dep/raylib.h"

class Button : public Container {
    public:
        std::function<void()> callback_on_click;

    private:
        void on_click() override {
            if (callback_on_click) callback_on_click();
        }

        void post_draw_tree() override {
            if (!is_hovered()) return;
            
            bool mouse_held = RayLib::IsMouseButtonDown(0);
            float alpha = mouse_held ? 0.3 : 0.2;

            Vector2 pos = position->get_global();
            RayLib::DrawRectangle(pos.x, pos.y, size->get().x, size->get().y, RayLib::ColorAlpha(RayLib::BLACK, alpha));
        }
        
};
