#include <cstdio>
#include "UI/Stack/Stack.h"
#include "Claire/Assert.h"

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
    if (allow_user_resize) do_user_resizing();

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

void HStack::do_user_resizing() {
    if (!RayLib::IsMouseButtonDown(RayLib::MOUSE_BUTTON_LEFT)) {
        grabee = nullptr;
    }

    if (grabee) {
        Vector2 mouse_delta = Vector2::from_ray(RayLib::GetMouseDelta());
        grabee->size->set_x(grabee->size->get().x + mouse_delta.x);

        return;
    }

    // Where are those borders anyways
    std::vector<int32_t> borders;
    std::vector<Container*> visible = visible_children();

    int32_t current_x = 0;
    for (auto child : visible) {
        current_x += child->size->get().x;

        borders.push_back(current_x);
    }

    ssize_t border = -1;
    Vector2 mouse_pos = Vector2::from_ray(RayLib::GetMousePosition());
    mouse_pos = mouse_pos - position->get_global();

    for (size_t i = 0; i < borders.size(); i++) {
        // If not around the border, ignore
        if (!mouse_pos.in_rectangle(
            { borders[i] - 5, 0 },
            { 11, size->get().y }
        )) continue;

        border = i;
        break;
    }

    if (border == -1) {
        RayLib::AwesomeSetMouseCursor(RayLib::MouseCursor::MOUSE_CURSOR_DEFAULT);
        return;
    }

    RayLib::AwesomeSetMouseCursor(RayLib::MouseCursor::MOUSE_CURSOR_RESIZE_EW);

    if (!RayLib::IsMouseButtonDown(RayLib::MOUSE_BUTTON_LEFT)) return;

    // NOW WE ARE HOVERING OVER A BORDER................
    ASSERT((size_t)border + 1 < visible.size(), "Hey who are you grabbing....?");

    Container* before = visible[border];
    Container* after = visible[border + 1];

    printf("Before strat: %i\n", before->size->strategy_x);
    printf("Aftr strat: %i\n", after->size->strategy_x);

    Container* target = before;

    // Does this even work at all???? If we force the right one doesn't it grow
    // from the left?? IDFK!!
    if (target->size->strategy_x != SizeStrategy::FORCE) target = after;

    // TODO: Make it force????
    ASSERT(target->size->strategy_x == SizeStrategy::FORCE, "TODO: Force somebody");

    grabee = target;
}