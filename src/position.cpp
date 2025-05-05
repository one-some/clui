#include "position.h"
#include "vector2.h"
#include "UI/Container/Container.h"

Vector2 Position::get_local() {
    switch (strategy) {
        case PositionStrategy::RELATIVE:
            return raw;
        case PositionStrategy::CENTER:
            // TODO: ASSERT PARENT
            return (owner->parent->size->get() - owner->size->get()) / 2;
    }

    // TODO: ASSERT FALSE
    return {0, 0};
}

Vector2 Position::get_global() {
    Container* target = owner;
    Vector2 pos = {0, 0};

    while (target) {
        pos = pos + target->position->get_local();
        target = target->parent;
    }
    
    return pos;
}
