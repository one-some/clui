#include "container.h"

void Container::add_child(Container* child) {
    // TODO: Can't have parent already
    child->parent = this;
    children.push_back(child);
}

Vector2 Container::global_position() {
    Vector2 pos = {0, 0};
    Container* target = this;

    while (target) {
        pos = pos + target->position->evaluate_local(target);
        target = target->parent;
    }
    
    return pos;
}

void Container::draw_tree() {
    draw_self();

    for (auto child : children) {
        child->draw_tree();
    }
}

void Container::propagate_mouse_motion(Vector2 pos) {
    // Update mouse
    bool in = pos.in_rectangle(global_position(), size);
    _is_hovered = in;
    on_hover_change(_is_hovered);

    for (auto child : children) {
        child->propagate_mouse_motion(pos);
    }
}

void Container::propagate_click() {
    if (is_hovered()) on_click();

    for (auto child : children) {
        child->propagate_click();
    }
}
