#pragma once
#include <vector>
#include <cstdint>

struct Vector2 {
    int32_t x;
    int32_t y;

    Vector2 operator+(const Vector2& that) {
        return {x + that.x, y + that.y};
    }

    bool in_rectangle(Vector2 pos, Vector2 size) {
        if (this->y - pos.y < 0) return false;
        if (this->y - pos.y > size.y) return false;

        if (this->x - pos.x < 0) return false;
        if (this->x - pos.x > size.x) return false;

        return true;
    }
};

enum PositionStrategy {
    RELATIVE,
    CENTER,
};

// TODO: SPLIT THE FILES!
class Container;

class Position {
    public:
        int32_t x = 0;
        int32_t y = 0;
        PositionStrategy strategy;

        Position(PositionStrategy strat) {
            strategy = strat;
        }

        Vector2 evaluate_local(Container* container);
};

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
        virtual void on_click() { };
};