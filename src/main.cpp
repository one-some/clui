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
#include "color.h"

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

class TextLabel : public Container {
    public:
        const char* text = "Test";
        Ray::Font* font = nullptr;

        TextLabel(const char* _text, Ray::Font* _font) {
            this->text = _text;
            this->font = _font;
        }

        void draw_self() override {
            if (!font) return;
            
            Vector2 pos = position->get_global();

            Ray::DrawTextEx(
                *font,
                text,
                { (float)pos.x, (float)pos.y },
                (float)font->baseSize,
                1,
                Ray::BLACK
            );
        }
};

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
            printf("CLICK!\n");
        }
};

class Stack : public Container {
    private:
        virtual void reposition_children() { }

        void on_child_added(Container* child) override {
            reposition_children();
        }
};

class HStack : public Stack {
    private:
        void reposition_children() override {
            for (size_t i=0; i < children.size(); i++) {
                children[i]->size->set_x(size->get().x);
                children[i]->size->set_y(size->get().y / (float)children.size());

                children[i]->position->set_y(size->get().y / (float)children.size() * i);
            }
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

    auto rect = Button();
    rect.position->strategy = PositionStrategy::CENTER;
    rect.size->set_raw({200, 200});
    root.add_child(&rect);

    auto rect2 = TextureRect("claire.png");
    // rect2->size = {100, 100};
    rect2.position->strategy = PositionStrategy::CENTER;
    rect.add_child(&rect2);

    auto font = Ray::LoadFont("tnr.ttf");
    auto text = TextLabel("Hellaoo world", &font);
    rect.add_child(&text);

    auto rect3 = ColorRect();
    rect3.color = Color(0xFF00FF).to_ray();
    rect3.size->set_raw({100, 100});
    rect2.add_child(&rect3);

    auto stack = HStack();
    stack.size->set_raw({100, 300});
    stack.size->strategy_y = SizeStrategy::EXPAND;
    root.add_child(&stack);

    auto sc1 = ColorRect();
    sc1.color = Color(0xFF0000).to_ray();
    stack.add_child(&sc1);

    auto sc2 = ColorRect();
    sc2.color = Color(0x00FF00).to_ray();
    stack.add_child(&sc2);

    auto sc3 = ColorRect();
    sc3.color = Color(0x0000FF).to_ray();
    stack.add_child(&sc3);

    while (!Ray::WindowShouldClose()) {
        root.size->set_x(Ray::GetRenderWidth());
        root.size->set_y(Ray::GetRenderHeight());

        root.propagate_mouse_motion({Ray::GetMouseX(), Ray::GetMouseY()});
        if (Ray::IsMouseButtonPressed(0)) root.propagate_click();

        Ray::BeginDrawing();
        Ray::ClearBackground(Colors::BG.to_ray());

        // if (frames % 2 == 0) rect->position.x += 1;

        root.draw_tree();

        Ray::EndDrawing();
        frames++;
    }

    Ray::CloseWindow();

    return 0;
}
