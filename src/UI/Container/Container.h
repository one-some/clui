#pragma once
#include <sys/types.h>
#include <stdio.h>
#include <vector>

#include <memory>

#include "position.h"
#include "size.h"
#include "vector2.h"

class Container {
    public:
        // Don't allow setting the whole position...
        std::unique_ptr<Position> const position = std::make_unique<Position>(Position(this));
        std::unique_ptr<Size> const size = std::make_unique<Size>(Size(this));

        // TODO: Private?
        Container* parent = nullptr;
        std::vector<std::unique_ptr<Container>> children;

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

    
    protected:
        bool _is_hovered = false;

        virtual void on_hover_change(bool is_hovered) { };
        virtual void on_child_added(std::unique_ptr<Container>& child) { };
        virtual void on_click() { };

        virtual void pre_draw_tree() { };
        virtual void post_draw_tree() { };
};