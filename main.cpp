#include <stdio.h>
namespace Ray {
    #include "raylib.h"
}
#include <cstdint>
#include <vector>
#include <iostream>
#include <memory>


enum PositionStrategy {
    ABSOLUTE,
    CENTER,
};

struct Vector2 {
    uint32_t x;
    uint32_t y;
};

class Color {
    public:
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;

        Color(uint32_t color) {
            this->b = color & 0xFF;
            this->g = color >> 8 & 0xFF;
            this->r = color >> 16 & 0xFF;
        }

        Ray::Color to_ray() {
            return { this->r, this->g, this->b, 0xFF };
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

        Vector2 evaluate() {
            // TODO: EVERYTHING
            return { this->x, this->y };
        }
};

class Container {
    public:
        std::vector<Container*> children;

        void draw_tree() {
            for (auto child : children) {
                child->draw_tree();
            }

            this->draw_self();
        }

        virtual void draw_self() { }
};

class RectContainer : public Container {
    public:
        Position* position = new Position(PositionStrategy::ABSOLUTE);
        Vector2 size = {0, 0};
};

class TextureRect : public RectContainer {
    public:
        std::unique_ptr<Ray::Texture2D> texture;

        TextureRect(const char* file_name) {
            this->texture = std::make_unique<Ray::Texture2D>(Ray::LoadTexture(file_name));
        }

        void draw_self() override {
            if (!this->texture) return;

            Vector2 pos = this->position->evaluate();
            // TODO: SIZE?
            Ray::DrawTexture(*this->texture, pos.x, pos.y, Ray::WHITE);
        }
};

class ColorRect : public RectContainer {
    public:
        Ray::Color color = Ray::RED;

        void draw_self() override {
            Vector2 pos = this->position->evaluate();
            Ray::DrawRectangle(pos.x, pos.y, this->size.x, this->size.y, this->color);
        }
};

int main() {
    auto root = new Container();

    // SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    Ray::InitWindow(500, 500, "clui test");
    Ray::SetTargetFPS(60);

    auto rect = new ColorRect();
    root->children.push_back(rect);
    rect->size = {200, 200};

    auto rect2 = new TextureRect("claire.png");
    rect2->size = {100, 100};
    rect2->position->x = 100;
    root->children.push_back(rect2);

    while (!Ray::WindowShouldClose()) {
        Ray::BeginDrawing();

        Ray::ClearBackground(Colors::BG.to_ray());

        root->draw_tree();

        Ray::EndDrawing();
    }

    Ray::CloseWindow();

    return 0;
}
