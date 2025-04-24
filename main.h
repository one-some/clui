#pragma once
#include <vector>
#include <cstdint>

struct Vector2 {
    int32_t x;
    int32_t y;

    Vector2 operator+(const Vector2& that) {
        return {x + that.x, y + that.y};
    }
};

enum PositionStrategy {
    RELATIVE,
    CENTER,
};

// TODO: SPLIT THE FILES!
class Container;

class Position {
    public:
        int32_t x = 0;
        int32_t y = 0;
        PositionStrategy strategy;

        Position(PositionStrategy strat) {
            strategy = strat;
        }

        Vector2 evaluate_local(Container* container);
};

class Container {
    public:
        std::vector<Container*> children;
        Container* parent;
        Position* position = new Position(PositionStrategy::RELATIVE);
        Vector2 size = {0, 0};

        void add_child(Container* child);
        void draw_tree(Vector2 at);
        virtual void draw_self(Vector2 at) { }
};