#pragma once
#include <sys/types.h>
#include <stdio.h>
#include <vector>

#include <map>
#include <typeindex>
#include <memory>

#include "color.h"
#include "UI/Events/Events.h"
#include "position.h"
#include "size.h"
#include "vector2.h"
#include "Claire/String.h"

enum class RenderSection {
    CULL,
    DRAW,
};

class ContainerDecoration {
public:
    int32_t border_top_px = 0;
    int32_t border_bottom_px = 0;
    int32_t border_left_px = 0;
    int32_t border_right_px = 0;

    Color border_color = Color(0xAAAAAA);
};

class Container {
public:
    // Don't allow setting the whole position...
    std::unique_ptr<Position> const position = std::make_unique<Position>(Position(this));
    std::unique_ptr<Size> const size = std::make_unique<Size>(Size(this));
    std::unique_ptr<Vector2> const scroll_offset = std::make_unique<Vector2>();
    RenderSection render_section = RenderSection::CULL;

    bool allow_scroll = false;

    // TODO: Private?
    std::unique_ptr<ContainerDecoration> decoration;
    Container* parent = nullptr;
    std::vector<std::unique_ptr<Container>> children;
    String debug_name;

    static Container* focused_element;

    virtual ~Container() {
        if (is_focused()) focused_element = nullptr;
    }

    Container* add_child(std::unique_ptr<Container> child);
    void remove_child(Container* child);

    template<typename T, typename... Args>
    T* create_child(Args&&... args) {
        static_assert(std::is_base_of<Container, T>::value, "Cant create whatever that is as a child");

        auto new_child = std::make_unique<T>(std::forward<Args>(args)...);
        T* raw_ptr = new_child.get();
        add_child(std::move(new_child));
        return raw_ptr;
    }

    virtual void on_input() { };

    inline bool is_focused() { return Container::focused_element == this; }
    bool is_hovered() { return last_hovered; }
    virtual bool manages_child_size() { return false; }

    virtual void draw_tree(Optional<RayLib::Rectangle> parent_scissor = Optional<RayLib::Rectangle>());
    virtual void draw_self() { }

    virtual std::vector<Container*> visible_children() {
        std::vector<Container*> out;
        for (auto &child : children) {
            out.push_back(child.get());
        }
        return out;
    }

    Vector2 get_draw_position() {
        Vector2 pos = position->get_local();
        // Container* target = parent;

        if (parent) {
            pos = pos + parent->get_draw_position();
        }

        if (render_section == RenderSection::DRAW) {
            pos = pos + *scroll_offset;
        }

        return pos;
    }

    template <typename TEvent>
    void register_handler(std::function<void(TEvent&)> handler) {
        const auto type = std::type_index(typeid(TEvent));
        object_event_handlers[type] = [handler](Event& event) {
            handler(static_cast<TEvent&>(event));
        };
    }

    void dispatch_event(Event& event) {
        const auto& type_info = typeid(event);
        const auto type = std::type_index(type_info);

        // Don't propagate events that aren't hovered over.
        // TODO: Maybe move into event?
        // TODO: Maybe turn the class registration into a vector of functions instead of one....
        // then the base class (ex Container) could implement base stuff like this...
        if (type_info == typeid(ClickEvent)) {
            if (!is_hovered()) return;

            // TODO: Focus chain
            Container::focused_element = this;
        }

        if (type_info == typeid(MouseMotionEvent)) {
            auto& squeak = (MouseMotionEvent&)event;
            bool hovered = Vector2({squeak.x, squeak.y}).in_rectangle(
                // get_draw_position(),
                position->get_global(),
                size->get()
            );

            if (hovered != last_hovered) {
                last_hovered = hovered;
                auto hover_event = MouseHoverChangeEvent(hovered);
                dispatch_event(hover_event);
            }
        }

        if (type_info == typeid(WheelEvent) && !is_hovered()) return;
        

        if (auto it = object_event_handlers.find(type); it != object_event_handlers.end()) {
            it->second(event);
        }

        if (auto it = class_event_handlers.find(type); event.do_default && it != class_event_handlers.end()) {
            it->second(event);
        }

        // Unhandled....
        if (!event.do_propagate) return;

        for (auto& child : children) {
            event.reset();
            child->dispatch_event(event);
        }
    }

protected:
    bool last_hovered = false;

    std::map<std::type_index, std::function<void(Event&)>> object_event_handlers;
    std::map<std::type_index, std::function<void(Event&)>> class_event_handlers = {
        // RLY UGLY Default setup for base class
        {
            std::type_index(typeid(WheelEvent)),
            [this](Event& event) { on_wheel(static_cast<WheelEvent&>(event)); }
        }
    };


    virtual void pre_draw_tree() { };
    virtual void post_draw_tree() { };

    template<typename TEvent, typename TClass>
    void register_class_handler(void (TClass::*handler)(TEvent&)) {
        const auto type = std::type_index(typeid(TEvent));

        class_event_handlers[type] = [this, handler](Event& event) {
            (static_cast<TClass*>(this)->*handler)(static_cast<TEvent&>(event));
        };
    }

private:
    void on_wheel(WheelEvent& event) {
        if (!allow_scroll) return;

        scroll_offset->y += event.delta_y * 16 * 3;
        if (scroll_offset->y > 0) scroll_offset->y = 0;
    }
};