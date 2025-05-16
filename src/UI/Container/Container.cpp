#include "UI/Container/Container.h"

void Container::add_child(Container* child) {
    // TODO: Can't have parent already
    child->parent = this;
    children.push_back(std::unique_ptr<Container>(child));

    on_child_added(child);
}

void Container::draw_tree() {
    Vector2 pos = position->get_global();
    RayLib::BeginScissorMode(pos.x, pos.y, size->get().x, size->get().y);
    draw_self();
    RayLib::EndScissorMode();

    for (const auto& child : children) {
        child->draw_tree();
    }
}

void Container::propagate_mouse_motion(Vector2 pos) {
    // Update mouse
    bool in = pos.in_rectangle(position->get_global(), size->get());
    _is_hovered = in;
    on_hover_change(_is_hovered);

    for (const auto& child : children) {
        child->propagate_mouse_motion(pos);
    }
}

void Container::propagate_click() {
    if (is_hovered()) on_click();

    for (const auto& child : children) {
        child->propagate_click();
    }
}
