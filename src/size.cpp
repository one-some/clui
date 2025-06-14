#include "size.h"
#include "vector2.h"
#include "UI/Container/Container.h"

Vector2 Size::get() {
    if (!owner->parent) {
        // O_o
        // printf("Who are you: %s\n", owner->debug_name.as_c());
        return raw;
    }

    if (owner->parent && owner->parent->manages_child_size()) return raw;

    Vector2 out;

    switch (strategy_x) {
        case SizeStrategy::FORCE:
        case SizeStrategy::FLIMSY:
            out.x = raw.x;
            break;
        case SizeStrategy::EXPAND_TO_FILL:
            out.x = owner->parent->size->get().x;
            break;
    }

    switch (strategy_y) {
        case SizeStrategy::FORCE:
        case SizeStrategy::FLIMSY:
            out.y = raw.y;
            break;
        case SizeStrategy::EXPAND_TO_FILL:
            out.y = owner->parent->size->get().y;
            break;
    }

    // TODO: ASSERT FALSE
    return out;
}
