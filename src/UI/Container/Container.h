#pragma once
#include <sys/types.h>
#include <stdio.h>
#include <vector>

#include <map>
#include <typeindex>
#include <memory>

#include "UI/Events/Events.h"
#include "position.h"
#include "size.h"
#include "vector2.h"
#include "Claire/String.h"

class Container {
public:
    // Don't allow setting the whole position...
    std::unique_ptr<Position> const position = std::make_unique<Position>(Position(this));
    std::unique_ptr<Size> const size = std::make_unique<Size>(Size(this));
    std::unique_ptr<Vector2> const scroll_offset = std::make_unique<Vector2>();

    // TODO: Private?
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

    void propagate_mouse_motion(Vector2 pos);
    void propagate_click();
    virtual void on_input() { };
    virtual void on_wheel(float delta) { };

    inline bool is_focused() { return Container::focused_element == this; }

    virtual void draw_tree();
    virtual void draw_self() { }

    bool is_hovered() { return _is_hovered; }

    virtual std::vector<Container*> visible_children() {
        std::vector<Container*> out;
        for (auto &child : children) {
            out.push_back(child.get());
        }
        return out;
    }

    Vector2 get_draw_position() {
        return position->get_global() + *scroll_offset;
    }

    virtual bool manages_child_size() { return false; }

    template <typename TEvent>
    void register_handler(std::function<void(TEvent&)> handler) {
        const auto type = std::type_index(typeid(TEvent));
        object_event_handlers[type] = [handler](Event& event) {
            handler(static_cast<TEvent&>(event));
        };
    }

    void dispatch_event(Event& event) {
        const auto type = std::type_index(typeid(event));

        if (auto it = object_event_handlers.find(type); it != object_event_handlers.end()) {
            it->second(event);
        }

        if (auto it = class_event_handlers.find(type); event.do_default && it != class_event_handlers.end()) {
            it->second(event);
        }

        // Unhandled....
        if (event.do_propagate) {
            event.reset();

            for (auto& child : children) {
                child->dispatch_event(event);
            }
        }
    }

protected:
    bool _is_hovered = false;


    virtual void on_hover_change(bool is_hovered) { };
    virtual void on_child_added(std::unique_ptr<Container>& child) { };
    virtual void on_click() { };

    virtual void pre_draw_tree() { };
    virtual void post_draw_tree() { };

    template<typename TEvent, typename TClass>
    void register_class_handler(void (TClass::*handler)(TEvent&)) {
        const auto type = std::type_index(typeid(TEvent));

        class_event_handlers[type] = [this, handler](Event& event) {
            (static_cast<TClass*>(this)->*handler)(static_cast<TEvent&>(event));
        };
    }

    std::map<std::type_index, std::function<void(Event&)>> object_event_handlers;
    std::map<std::type_index, std::function<void(Event&)>> class_event_handlers;
};