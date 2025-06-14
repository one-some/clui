// #define DEBUG_DRAW

#include "Claire/Assert.h"
#include "UI/Container/Container.h"
#include <functional>

#define DEBUG_DRAW

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

void Container::draw_tree(Optional<RayLib::Rectangle> parent_scissor) {
    pre_draw_tree();

    // Vector2 pos = position->get_global();

    render_section = RenderSection::CULL;

    Vector2 pos = get_draw_position();
    Vector2 s = size->get();

    RayLib::Rectangle scissor = { pos.x, pos.y, s.x, s.y };
    if (parent_scissor) {
        scissor = RayLib::GetCollisionRec(scissor, *parent_scissor);
    }

    // TODO: Scissor optional
    RayLib::BeginScissorMode(scissor.x, scissor.y, scissor.width, scissor.height);

    #ifdef DEBUG_DRAW
        RayLib::DrawRectangleLines(scissor.x, scissor.y, scissor.width, scissor.height, {0xFF, 0x00, 0xFF, 0x88});
        if (is_hovered()) {
            RayLib::DrawRectangle(scissor.x, scissor.y, scissor.width, scissor.height, {0xFF, 0x00, 0xFF, 0x0F});
        }
    #endif

    render_section = RenderSection::DRAW;


    draw_self();
    RayLib::EndScissorMode();

    for (const auto& child : children) {
        child->draw_tree(Optional<RayLib::Rectangle>(scissor));
    }

    post_draw_tree();
}