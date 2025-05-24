// #define DEBUG_DRAW

#include "UI/Container/Container.h"
#include <functional>

Container* Container::focused_element = nullptr;

void Container::add_child(std::unique_ptr<Container> child) {
    // TODO: Can't have parent already
    child->parent = this;
    children.push_back(std::move(child));

    on_child_added(children.back());
}

void Container::remove_child(std::unique_ptr<Container> child) {
    children.erase(std::remove_if(
        children.begin(),
        children.end(),
        [&child](const std::unique_ptr<Container>& maybe_child) {
            return maybe_child == child;
        }),
        children.end()
    );
}

void Container::draw_tree() {
    pre_draw_tree();

    Vector2 pos = position->get_global();

    // TODO: Scissor optional
    RayLib::BeginScissorMode(pos.x, pos.y, size->get().x, size->get().y);

#ifdef DEBUG_DRAW
    Vector2 s = size->get();
    RayLib::DrawRectangleLines(pos.x, pos.y, s.x, s.y, {0xFF, 0x00, 0xFF, 0x88});
#endif

    draw_self();

    RayLib::EndScissorMode();

    for (const auto& child : children) {
        child->draw_tree();
    }

    post_draw_tree();
}

void Container::propagate_mouse_motion(Vector2 pos) {
    // Update mouse
    printf("YEA IIM PROPMOUSE\n");
    bool in = pos.in_rectangle(position->get_global(), size->get());
    _is_hovered = in;
    on_hover_change(_is_hovered);

    for (const auto& child : children) {
        child->propagate_mouse_motion(pos);
    }
}

void Container::propagate_click() {
    if (is_hovered()) {
        Container::focused_element = this;
        on_click();
    }

    for (const auto& child : children) {
        child->propagate_click();
    }
}