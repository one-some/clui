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
        int32_t x = 0;
        int32_t y = 0;
        PositionStrategy strategy;

        Position(PositionStrategy strat) {
            strategy = strat;
        }

        Vector2 evaluate_local(Container* container);
};

// MAYBE A WORSE HACK!!
#include "container.h"