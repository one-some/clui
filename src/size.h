#pragma once
#include <cstdint>
#include "vector2.h"

// HACK
class Container;

enum SizeStrategy {
    FLIMSY,
    FORCE,
    EXPAND_TO_FILL,
};

class Size {
    public:
        Size(Container* owner) {
            this->owner = owner;
        }

        SizeStrategy strategy_x = SizeStrategy::FLIMSY;
        SizeStrategy strategy_y = SizeStrategy::FLIMSY;

        Vector2 get();

        void set_raw(Vector2 raw) { this->raw = raw; }
        void set_x(int32_t x) { this->raw.x = x; }
        void set_y(int32_t y) { this->raw.y = y; }
    
    private:
        Container* owner;
        Vector2 raw = {0, 0};
};

// HACK
#include "UI/Container/Container.h"