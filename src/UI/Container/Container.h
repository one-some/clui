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

        virtual ~Container() { }

        void add_child(std::unique_ptr<Container> child);
        void remove_child(std::unique_ptr<Container> child);

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