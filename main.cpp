#include <stdio.h>
namespace Ray {
    #include "raylib.h"
}
#include <cstdint>
#include <vector>
#include <iostream>
#include <memory>


enum PositionStrategy {
    RELATIVE,
    CENTER,
};

struct Vector2 {
    uint32_t x;
    uint32_t y;

    Vector2 operator+(Vector2& that) {
        return {x + that.x, y + that.y};
    }
};

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

class Position {
    public:
        uint32_t x = 0;
        uint32_t y = 0;
        PositionStrategy strategy;

        Position(PositionStrategy strat) {
            strategy = strat;
        }

        Vector2 evaluate(Vector2 self_size, Vector2 parent_size) {
            switch (strategy) {
                case PositionStrategy::RELATIVE:
                    Vector2 parent_pos_offset = { 0, 0 };
                    Container parent = 
                    while ()
                    // TODO!
                    return { x, y };

                case PositionStrategy::CENTER:
                    return {
                        (parent_size.x - self_size.x) / 2,
                        (parent_size.y -self_size.y) / 2,
                    };
            }

            // TODO: ASSERT FALSE
            return {0, 0};
        }
};

class Container {
    public:
        std::vector<Container*> children;
        Container* parent;
        Position* position = new Position(PositionStrategy::RELATIVE);
        Vector2 size = {0, 0};

        void add_child(Container* child) {
            // TODO: Can't have parent already
            child->parent = this;
            children.push_back(child);
        }

        void draw_tree() {
            draw_self();

            for (auto child : children) {
                child->draw_tree();
            }
        }

        virtual void draw_self() { }
};

class TextureRect : public Container {
    public:
        std::unique_ptr<Ray::Texture2D> texture;

        TextureRect(const char* file_name) {
            texture = std::make_unique<Ray::Texture2D>(Ray::LoadTexture(file_name));
            size = {
                (uint32_t) texture->width,
                (uint32_t) texture->height
            };
        }

        void draw_self() override {
            if (!texture) return;

            Vector2 pos = position->evaluate(size, parent->size);

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
            Vector2 pos = position->evaluate(size, parent->size);
            Ray::DrawRectangle(pos.x, pos.y, size.x, size.y, color);
        }
};

int main() {
    auto root = new Container();

    // SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    Ray::InitWindow(500, 500, "clui test");
    Ray::SetTargetFPS(60);

    auto rect = new ColorRect();
    rect->size = {200, 200};
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
        Ray::BeginDrawing();

        Ray::ClearBackground(Colors::BG.to_ray());

        root->draw_tree();

        Ray::EndDrawing();
    }

    Ray::CloseWindow();

    return 0;
}
