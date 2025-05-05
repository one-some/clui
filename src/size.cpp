#include "size.h"
#include "vector2.h"
#include "UI/Container/Container.h"

Vector2 Size::get() {
    Vector2 out;

    switch (strategy_x) {
        case SizeStrategy::FORCE:
        case SizeStrategy::ABSOLUTE:
            out.x = raw.x;
            break;
        case SizeStrategy::EXPAND:
            out.x = owner->parent->size->get().x;
            break;
    }

    switch (strategy_y) {
        case SizeStrategy::FORCE:
        case SizeStrategy::ABSOLUTE:
            out.y = raw.y;
            break;
        case SizeStrategy::EXPAND:
            out.y = owner->parent->size->get().y;
            break;
    }

    // TODO: ASSERT FALSE
    return out;
}
