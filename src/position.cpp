#include "position.h"
#include "vector2.h"
#include "container.h"

Vector2 Position::evaluate_local(Container* container) {
    switch (strategy) {
        case PositionStrategy::RELATIVE:
            return Vector2 { x, y };
        case PositionStrategy::CENTER:
            // TODO: ASSERT PARENT
            return Vector2 {
                (container->parent->size.x - container->size.x) / 2,
                (container->parent->size.y - container->size.y) / 2,
            };
    }

    // TODO: ASSERT FALSE
    return {0, 0};
}
