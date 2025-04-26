#pragma once
#include <cstdint>

#include "vector2.h"

// HACK
class Container;

enum PositionStrategy {
    RELATIVE,
    CENTER,
};

class Position {
    public:
        Position(Container* owner) {
            this->owner = owner;
        }

        PositionStrategy strategy = PositionStrategy::RELATIVE;

        Vector2 get_local();
        Vector2 get_global();

        void set_raw(Vector2 raw) { this->raw = raw; }
        void set_x(int32_t x) { this->raw.x = x; }
        void set_y(int32_t y) { this->raw.y = y; }
    private:
        Container* owner;
        Vector2 raw = {0, 0};
};

// MAYBE A WORSE HACK!!
#include "container.h"