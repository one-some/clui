#include "assert.h"
#include <stdio.h>
namespace Ray {
    #include "raylib.h"
}
#include <cstdint>
#include <vector>
#include <iostream>
#include <memory>
#include <ranges>
#include <string.h>

#include "container.h"
#include "textedit.h"
#include "color.h"
#include "cpp.h"
#include "file.h"
#include "string.h"
#include "stack.h"
#include "tabcontainer.h"
#include "fontglobal.h"

std::unique_ptr<Ray::Font> Font::the_raw;

class TextureRect : public Container {
    public:
        std::unique_ptr<Ray::Texture2D> texture;

        TextureRect(const char* file_name) {
            texture = std::make_unique<Ray::Texture2D>(Ray::LoadTexture(file_name));
            size->set_raw({ texture->width, texture->height });
        }

        void draw_self() override {
            if (!texture) return;

            Vector2 pos = position->get_global();
            Ray::DrawTexturePro(
                *texture,
                { 0.0, 0.0, (float)texture->width, (float)texture->height },
                Vector2::to_ray_rect(pos, size->get()),
                { 0, 0 },
                0.0,
                Ray::WHITE
            );
        }
};

class ColorRect : public Container {
    public:
        Ray::Color color = Ray::RED;

        void draw_self() override {
            Vector2 pos = position->get_global();
            Ray::DrawRectangle(pos.x, pos.y, size->get().x, size->get().y, color);
        }
};

int main() {
    // SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    Ray::SetConfigFlags(Ray::FLAG_WINDOW_RESIZABLE);
    Ray::InitWindow(500, 500, "clui test");
    Ray::SetTargetFPS(60);
    uint64_t frames = 0;

    auto root = Container();
    root.size->set_raw({ 500, 500 });

    auto stack = VStack();
    stack.size->set_raw({300, 300});
    stack.size->strategy_x = SizeStrategy::EXPAND;
    stack.size->strategy_y = SizeStrategy::EXPAND;
    root.add_child(&stack);

    auto sc1 = ColorRect();
    sc1.color = Color(0xFF0000).to_ray();
    sc1.size->strategy_y = SizeStrategy::FORCE;
    sc1.size->set_y(30);
    stack.add_child(&sc1);

    auto tabs = TabContainer();
    stack.add_child(&tabs);

    auto sc2 = TextEdit();
    tabs.add_child(&sc2);

    auto sc3 = ColorRect();
    sc3.color = Color(0x0000FF).to_ray();
    sc3.size->strategy_y = SizeStrategy::FORCE;
    sc3.size->set_y(20);
    stack.add_child(&sc3);

    while (!Ray::WindowShouldClose()) {
        root.size->set_x(Ray::GetRenderWidth());
        root.size->set_y(Ray::GetRenderHeight());

        root.propagate_mouse_motion({Ray::GetMouseX(), Ray::GetMouseY()});
        if (Ray::IsMouseButtonPressed(0)) root.propagate_click();

        Ray::BeginDrawing();
        Ray::ClearBackground(Colors::BG);

        // if (frames % 2 == 0) rect->position.x += 1;

        root.draw_tree();

        Ray::EndDrawing();
        frames++;
    }

    Ray::CloseWindow();

    return 0;
}
