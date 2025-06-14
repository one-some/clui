#pragma once
#include <cstdint>

#include "vector2.h"
#include "Claire/BitEnum.h"

// HACK
class Container;

enum class XPositionStrategy {
    RAW,
    CENTER,
    LEFT,
    RIGHT,
};

enum class YPositionStrategy {
    RAW,
    CENTER,
    TOP,
    BOTTOM,
};

class Position {
    public:
        Position(Container* owner) {
            this->owner = owner;
        }

        XPositionStrategy x_strategy = XPositionStrategy::RAW;
        YPositionStrategy y_strategy = YPositionStrategy::RAW;

        Vector2 get_local();
        Vector2 get_global();

        void set_raw(Vector2 _raw) { this->raw = _raw; }
        void set_x(int32_t x) { this->raw.x = x; }
        void set_y(int32_t y) { this->raw.y = y; }
    private:
        Container* owner;
        Vector2 raw = {0, 0};
};

// MAYBE A WORSE HACK!!
#include "UI/Container/Container.h"