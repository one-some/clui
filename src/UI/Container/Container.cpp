// #define DEBUG_DRAW

#include "Claire/Assert.h"
#include "UI/Container/Container.h"
#include <functional>

// #define DEBUG_DRAW

Container* Container::focused_element = nullptr;

Container* Container::add_child(std::unique_ptr<Container> child) {
    ASSERT(!!child, "Gimmie");

    // TODO: Can't have parent already
    child->parent = this;
    children.push_back(std::move(child));

    auto event = AddChildEvent(children.back().get());
    dispatch_event(event);
    return children.back().get();
}

void Container::remove_child(Container* child) {
    children.erase(std::remove_if(
        children.begin(),
        children.end(),
        [&child](const std::unique_ptr<Container>& maybe_child) {
            return maybe_child.get() == child;
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
    if (is_hovered()) {
        Vector2 s = size->get();
        //RayLib::DrawRectangleLines(pos.x, pos.y, s.x, s.y, {0xFF, 0x00, 0xFF, 0x88});
        RayLib::DrawRectangle(pos.x, pos.y, s.x, s.y, {0xFF, 0x00, 0xFF, 0x0F});
    }
#endif

    draw_self();

    RayLib::EndScissorMode();

    for (const auto& child : children) {
        child->draw_tree();
    }

    post_draw_tree();
}