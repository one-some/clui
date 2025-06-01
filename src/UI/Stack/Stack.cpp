#include <cstdio>
#include "UI/Stack/Stack.h"
#include "Claire/Assert.h"
#include "FrameManager/FrameManager.h"

int32_t Stack::calculate_min_size(Container* thing) {
    Stack* maybe_stack = dynamic_cast<Stack*>(thing);

    if (!maybe_stack) {
        // THAT'S NOT A STACK
        if (relevant_strat(thing->size.get()) == SizeStrategy::FORCE) {
            return relevant_axis(thing->size->get());
        }
        return 0;
    }

    // Its a stack....
    int32_t out = 0;

    for (auto& child : thing->visible_children()) {
        out += calculate_min_size(child);
    }

    return out;
}

void Stack::reposition_children() {
    if (relevant_strat(size.get()) == SizeStrategy::FLIMSY) {
        int32_t min_size = calculate_min_size(this);

        // if (can_shrink || min_size > relevant_axis(size->get())) {
        set_relevant(size.get(), min_size);
        // }
    }

    // If we're forcing, the budget MUST BE THIS....
    int32_t budget = relevant_axis(size->get());
    int32_t expandable_child_count = 0;

    for (auto& child : visible_children()) {
        auto child_strat = relevant_strat(child->size.get());
        if (
            child_strat == SizeStrategy::EXPAND_TO_FILL
            || (child_strat == SizeStrategy::FLIMSY && layout_style == StackLayout::EXPAND)
        ) {
            expandable_child_count++;
        } else {
            // FORCE OR (STACK AND FLIMSY)
            int32_t child_size = relevant_axis(child->size->get());
            budget -= child_size;
        }
    }

    if (budget < 0) budget = 0;

    int32_t current_pos = 0;
    for (auto& child : visible_children()) {
        // Make their secondary axis match ours
        set_inv_relevant(child->size.get(), inv_relevant_axis(size->get()));

        auto child_strat = relevant_strat(child->size.get());
        if (
            child_strat == SizeStrategy::EXPAND_TO_FILL
            || (child_strat == SizeStrategy::FLIMSY && layout_style == StackLayout::EXPAND)
        ) {
            // Gobble all space like HARDWARE.../teto
            int32_t size = 0;
            if (expandable_child_count > 0) {
                size = budget / expandable_child_count;
            }

            set_relevant(child->size.get(), size);
        }

        // Put them in the right place
        set_relevant(child->position.get(), current_pos);
        current_pos += relevant_axis(child->size->get());
    }
}

void Stack::do_user_resizing() {
    if (!RayLib::IsMouseButtonDown(RayLib::MOUSE_BUTTON_LEFT)) {
        grabee = nullptr;
    }

    if (grabee) {
        Vector2 mouse_delta = Vector2::from_ray(RayLib::GetMouseDelta());
        int sign = (dynamic_cast<VStack*>(this)) ? -1 : 1;

        set_relevant(
            grabee->size.get(),
            relevant_axis(grabee->size->get()) + (sign * relevant_axis(mouse_delta))
        );

        FrameManager::set_frame_cursor(drag_cursor());
        return;
    }

    // Where are those borders anyways
    std::vector<int32_t> borders;
    std::vector<Container*> visible = visible_children();

    int32_t current_pos = 0;
    for (auto& child : visible) {
        current_pos += relevant_axis(child->size->get());

        borders.push_back(current_pos);
    }

    ssize_t border = -1;
    Vector2 mouse_pos = Vector2::from_ray(RayLib::GetMousePosition());
    mouse_pos = mouse_pos - position->get_global();

    for (size_t i = 0; i < borders.size() - 1; i++) {
        // If not around the border, ignore
        if (!check_dragger_collision(mouse_pos, borders[i])) continue;

        border = i;
        break;
    }

    if (border == -1) return;

    FrameManager::set_frame_cursor(drag_cursor());

    if (!RayLib::IsMouseButtonDown(RayLib::MOUSE_BUTTON_LEFT)) return;

    // NOW WE ARE HOVERING OVER A BORDER................
    ASSERT((size_t)border + 1 < visible.size(), "Hey who are you grabbing....?");

    Container* before = visible[border];
    Container* after = visible[border + 1];

    Container* target = before;

    // Does this even work at all???? If we force the right one doesn't it grow
    // from the left?? IDFK!!
    if (relevant_strat(target->size.get()) != SizeStrategy::FORCE) target = after;

    // TODO: Make it force????
    ASSERT(relevant_strat(target->size.get()) == SizeStrategy::FORCE, "TODO: Force somebody");

    grabee = target;
}