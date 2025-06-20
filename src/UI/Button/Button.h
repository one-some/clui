#pragma once
#include <functional>
#include "UI/Container/Container.h"
#include "Dep/raylib.h"
#include "FrameManager/FrameManager.h"

class Button : public Container {
    public:
        std::function<void()> callback_on_mouse_down;

        Button() {
            register_class_handler<MouseDownEvent, Button>(&Button::on_mouse_down);
        }

    private:
        void on_mouse_down(MouseDownEvent& event) {
            if (event.button != MouseButton::LEFT) return;
            if (!callback_on_mouse_down) return;
            FrameManager::queue_operation(callback_on_mouse_down);
        }

        void post_draw_tree() override {
            if (!is_hovered()) return;

            FrameManager::set_frame_cursor(RayLib::MOUSE_CURSOR_POINTING_HAND);
            
            bool mouse_held = RayLib::IsMouseButtonDown(0);
            float alpha = mouse_held ? 0.3 : 0.2;

            Vector2 pos = position->get_global();
            RayLib::DrawRectangle(pos.x, pos.y, size->get().x, size->get().y, RayLib::ColorAlpha(RayLib::BLACK, alpha));
        }
        
};
