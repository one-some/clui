#include <cstdio>
#include "UI/Stack/Stack.h"

void VStack::reposition_children() {
    int32_t height_budget = size->get().y;
    int32_t flimsy_child_count = 0;

    for (auto child : visible_children()) {
        if (child->size->strategy_y != SizeStrategy::FORCE) {
            flimsy_child_count++;
            continue;
        }

        height_budget -= child->size->get().y;
    }

    int32_t current_y = 0;
    for (auto child : visible_children()) {
        child->size->set_x(size->get().x);
        
        if (child->size->strategy_y != SizeStrategy::FORCE) {
            child->size->set_y(height_budget / flimsy_child_count);
        }

        child->position->set_y(current_y);
        current_y += child->size->get().y;
    }
}

void HStack::reposition_children() {
    int32_t width_budget = size->get().x;
    int32_t flimsy_child_count = 0;

    for (auto child : visible_children()) {
        if (child->size->strategy_x != SizeStrategy::FORCE) {
            flimsy_child_count++;
            continue;
        }

        width_budget -= child->size->get().x;
    }

    int32_t current_x = 0;
    for (auto child : visible_children()) {
        child->size->set_y(size->get().y);
        
        if (child->size->strategy_x != SizeStrategy::FORCE) {
            child->size->set_x(width_budget / flimsy_child_count);
        }

        child->position->set_x(current_x);
        current_x += child->size->get().x;
    }
}