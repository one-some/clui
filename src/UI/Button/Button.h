#pragma once
#include <functional>
#include "UI/Container/Container.h"
#include "Dep/raylib.h"

class Button : public Container {
    public:
        std::function<void()> callback_on_click;

        void draw_tree() override {
            // Override draw_tree to change order..... a bit hacky

            for (const auto &child : children) {
                child->draw_tree();
            }

            if (!is_hovered()) return;
            
            bool mouse_held = RayLib::IsMouseButtonDown(0);
            float alpha = mouse_held ? 0.3 : 0.2;

            Vector2 pos = position->get_global();
            RayLib::DrawRectangle(pos.x, pos.y, size->get().x, size->get().y, RayLib::ColorAlpha(RayLib::BLACK, alpha));
        }
        
    private:
        void on_click() override {
            if (callback_on_click) callback_on_click();
        }
};
