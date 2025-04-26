#pragma once
#include <vector>

#include "position.h"
#include "vector2.h"

class Container {
    public:
        Position* position = new Position(PositionStrategy::RELATIVE);
        Vector2 size = {0, 0};
        // TODO: Private?
        Container* parent;
        std::vector<Container*> children;

        void add_child(Container* child);

        void propagate_mouse_motion(Vector2 pos);
        void propagate_click();

        virtual void draw_tree();
        virtual void draw_self() { }

        Vector2 global_position();
        
        bool is_hovered() { return _is_hovered; }
    
    private:
        bool _is_hovered = false;

        virtual void on_hover_change(bool is_hovered) { };
        virtual void on_child_added(Container* child) { };
        virtual void on_click() { };
};