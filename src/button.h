#pragma once
#include "container.h"

class Button : public Container {
    public:
        void draw_tree() override {
            // Override draw_tree to change order..... a bit hacky

            for (auto child : children) {
                child->draw_tree();
            }

            if (!is_hovered()) return;
            
            bool mouse_held = Ray::IsMouseButtonDown(0);
            float alpha = mouse_held ? 0.3 : 0.2;

            Vector2 pos = position->get_global();
            Ray::DrawRectangle(pos.x, pos.y, size->get().x, size->get().y, Ray::ColorAlpha(Ray::BLACK, alpha));
        }
        
    private:
        void on_click() override {
        }
};
