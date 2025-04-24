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

void Container::draw_tree(Vector2 at) {
    Vector2 pos = at + position->evaluate_local(this);
    draw_self(pos);

    for (auto child : children) {
        child->draw_tree(pos);
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

        void draw_self(Vector2 at) override {
            if (!texture) return;

            Ray::DrawTexturePro(
                *texture,
                { 0.0, 0.0, (float)texture->width, (float)texture->height },
                { (float)at.x, (float)at.y, (float)size.x, (float)size.y },
                { 0, 0 },
                0.0,
                Ray::WHITE
            );
        }
};

class ColorRect : public Container {
    public:
        Ray::Color color = Ray::RED;

        void draw_self(Vector2 at) override {
            Ray::DrawRectangle(at.x, at.y, size.x, size.y, color);
        }
};

int main() {
    // SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    Ray::SetConfigFlags(Ray::FLAG_WINDOW_RESIZABLE);
    Ray::InitWindow(500, 500, "clui test");
    Ray::SetTargetFPS(60);
    uint64_t frames = 0;

    auto root = new Container();
    root->size = { 500, 500 };

    auto rect = new ColorRect();
    rect->position->strategy = PositionStrategy::CENTER;
    rect->size = {200, 200};
    rect->position->x = 50;
    rect->position->y = 50;
    root->add_child(rect);

    auto rect2 = new TextureRect("claire.png");
    // rect2->size = {100, 100};
    rect2->position->strategy = PositionStrategy::CENTER;
    rect->add_child(rect2);

    auto rect3 = new ColorRect();
    rect3->color = Color(0xFF00FF).to_ray();
    rect3->size = {100, 100};
    rect2->add_child(rect3);

    while (!Ray::WindowShouldClose()) {
        root->size = {Ray::GetRenderWidth(), Ray::GetRenderHeight()};

        Ray::BeginDrawing();

        Ray::ClearBackground(Colors::BG.to_ray());

        // if (frames % 2 == 0) rect->position->x += 1;

        root->draw_tree({0, 0});

        Ray::EndDrawing();
        frames++;
    }

    Ray::CloseWindow();

    return 0;
}
