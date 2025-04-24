#include <stdio.h>
namespace Ray {
    #include "raylib.h"
}
#include <cstdint>
#include <vector>
#include <iostream>
#include <memory>
#include "main.h"
#include <string.h>

class Color {
    public:
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;

        Color(uint32_t color) {
            b = color & 0xFF;
            g = color >> 8 & 0xFF;
            r = color >> 16 & 0xFF;
        }

        Ray::Color to_ray() {
            return { r, g, b, 0xFF };
        }
};

namespace Colors {
    auto BG = Color(0x0000AA);
}

void Container::add_child(Container* child) {
    // TODO: Can't have parent already
    child->parent = this;
    children.push_back(child);
}

Vector2 Container::global_position() {
    Vector2 pos = {0, 0};
    Container* target = this;

    while (target) {
        pos = pos + target->position->evaluate_local(target);
        target = target->parent;
    }
    
    return pos;
}

void Container::draw_tree() {
    draw_self();

    for (auto child : children) {
        child->draw_tree();
    }
}

Vector2 Position::evaluate_local(Container* container) {
    switch (strategy) {
        case PositionStrategy::RELATIVE:
            return (Vector2) { x, y };
        case PositionStrategy::CENTER:
            // TODO: ASSERT PARENT
            return (Vector2) {
                (container->parent->size.x - container->size.x) / 2,
                (container->parent->size.y - container->size.y) / 2,
            };
    }

    // TODO: ASSERT FALSE
    return {0, 0};
}


class TextureRect : public Container {
    public:
        std::unique_ptr<Ray::Texture2D> texture;

        TextureRect(const char* file_name) {
            texture = std::make_unique<Ray::Texture2D>(Ray::LoadTexture(file_name));
            size = { texture->width, texture->height };
        }

        void draw_self() override {
            if (!texture) return;

            Vector2 pos = global_position();
            Ray::DrawTexturePro(
                *texture,
                { 0.0, 0.0, (float)texture->width, (float)texture->height },
                { (float)pos.x, (float)pos.y, (float)size.x, (float)size.y },
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
            Vector2 pos = global_position();
            Ray::DrawRectangle(pos.x, pos.y, size.x, size.y, color);
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
            
            Vector2 pos = global_position();

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

            Vector2 pos = global_position();
            Ray::DrawRectangle(pos.x, pos.y, size.x, size.y, Ray::ColorAlpha(Ray::BLACK, alpha));
        }
        
    private:
        void on_click() override {
            printf("CLICK!\n");
        }
};

void Container::propagate_mouse_motion(Vector2 pos) {
    // Update mouse
    bool in = pos.in_rectangle(global_position(), size);
    _is_hovered = in;
    on_hover_change(_is_hovered);

    for (auto child : children) {
        child->propagate_mouse_motion(pos);
    }
}

void Container::propagate_click() {
    if (is_hovered()) on_click();

    for (auto child : children) {
        child->propagate_click();
    }
}

int main() {
    // SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    Ray::SetConfigFlags(Ray::FLAG_WINDOW_RESIZABLE);
    Ray::InitWindow(500, 500, "clui test");
    Ray::SetTargetFPS(60);
    uint64_t frames = 0;

    auto root = new Container();
    root->size = { 500, 500 };

    auto rect = new Button();
    rect->position->strategy = PositionStrategy::CENTER;
    rect->size = {200, 200};
    rect->position->x = 50;
    rect->position->y = 50;
    root->add_child(rect);

    auto rect2 = new TextureRect("claire.png");
    // rect2->size = {100, 100};
    rect2->position->strategy = PositionStrategy::CENTER;
    rect->add_child(rect2);

    auto font = Ray::LoadFont("tnr.ttf");
    auto text = new TextLabel("Hella world", &font);
    rect->add_child(text);

    auto rect3 = new ColorRect();
    rect3->color = Color(0xFF00FF).to_ray();
    rect3->size = {100, 100};
    rect2->add_child(rect3);

    while (!Ray::WindowShouldClose()) {
        root->size = {Ray::GetRenderWidth(), Ray::GetRenderHeight()};

        root->propagate_mouse_motion({Ray::GetMouseX(), Ray::GetMouseY()});
        if (Ray::IsMouseButtonPressed(0)) root->propagate_click();

        Ray::BeginDrawing();
        Ray::ClearBackground(Colors::BG.to_ray());

        // if (frames % 2 == 0) rect->position->x += 1;

        root->draw_tree();

        Ray::EndDrawing();
        frames++;
    }

    Ray::CloseWindow();

    return 0;
}
