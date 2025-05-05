#pragma once
#include <vector>

#include "position.h"
#include "size.h"
#include "vector2.h"

class Container {
    public:
        // Don't allow setting the whole position...
        Position* const position = new Position(this);
        Size* const size = new Size(this);

        // TODO: Private?
        Container* parent = nullptr;
        std::vector<Container*> children;

        void add_child(Container* child);

        void propagate_mouse_motion(Vector2 pos);
        void propagate_click();

        virtual void draw_tree();
        virtual void draw_self() { }

        bool is_hovered() { return _is_hovered; }
        virtual std::vector<Container*> visible_children() { return children; }
    
    protected:
        bool _is_hovered = false;

        virtual void on_hover_change(bool is_hovered) { };
        virtual void on_child_added(Container* child) { };
        virtual void on_click() { };
};