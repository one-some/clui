#include "position.h"
#include "vector2.h"
#include "UI/Container/Container.h"
#include "Claire/Assert.h"

Vector2 Position::get_local() {
    Vector2 out = Vector2::zero();

    switch (x_strategy) {
        case XPositionStrategy::RAW:
            out.x = raw.x;
            break;
        case XPositionStrategy::CENTER:
            // TODO: ASSERT PARENT
            out.x = (owner->parent->size->get().x - owner->size->get().x) / 2;
            break;
        case XPositionStrategy::LEFT:
            out.x = 0;
            break;
        case XPositionStrategy::RIGHT:
            out.x = owner->parent->size->get().x - owner->size->get().x;
            break;
        default:
            ASSERT_NOT_REACHED("Invalid x strat");
    }

    switch (y_strategy) {
        case YPositionStrategy::RAW:
            out.y = raw.y;
            break;
        case YPositionStrategy::CENTER:
            // TODO: ASSERT PARENT
            out.y = (owner->parent->size->get().y - owner->size->get().y) / 2;
            break;
        case YPositionStrategy::TOP:
            out.y = 0;
            break;
        case YPositionStrategy::BOTTOM:
            out.y = owner->parent->size->get().y - owner->size->get().y;
            break;
        default:
            ASSERT_NOT_REACHED("Invalid y strat");
    }

    return out;
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
