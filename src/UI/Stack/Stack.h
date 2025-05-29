#pragma once
#include <cstdio>
#include "UI/Container/Container.h"

enum class StackLayout {
    LAYOUT_EXPAND,
    LAYOUT_STACK
};

class Stack : public Container {
    public:
        StackLayout layout_style = StackLayout::LAYOUT_EXPAND;
        bool allow_user_resize = false;

        virtual ~Stack() = default;

        void draw_self() override {
            if (allow_user_resize) do_user_resizing();

            // WE ARE EVIL
            reposition_children();
        }

    protected:
        Container* grabee = nullptr;

        virtual RayLib::MouseCursor drag_cursor() const = 0;

        int32_t calculate_min_size(Container* thing);

        virtual int32_t relevant_axis(Vector2 v) const = 0;
        virtual int32_t inv_relevant_axis(Vector2 v) const = 0;
        virtual SizeStrategy relevant_strat(Size* s) const = 0;
        virtual void set_relevant(Size* s, int32_t a) const = 0;
        virtual void set_inv_relevant(Size* s, int32_t a) const = 0;
        virtual void set_relevant(Position* s, int32_t a) const = 0;
        virtual void set_inv_relevant(Position* s, int32_t a) const = 0;

        virtual bool check_dragger_collision(Vector2 position, int32_t border) = 0;

        void on_child_added(std::unique_ptr<Container>& child) override {
            reposition_children();
        }

        void reposition_children();
        void do_user_resizing();
};

class VStack : public Stack {
    protected:
        inline RayLib::MouseCursor drag_cursor() const override final { return RayLib::MOUSE_CURSOR_RESIZE_NS; }

        inline int32_t relevant_axis(Vector2 v) const override final { return v.y; };
        inline int32_t inv_relevant_axis(Vector2 v) const override final { return v.x; };
        inline SizeStrategy relevant_strat(Size* s) const override final { return s->strategy_y; }
        void set_relevant(Size* s, int32_t a) const override final { s->set_y(a); }
        void set_inv_relevant(Size* s, int32_t a) const override final { s->set_x(a); }
        void set_relevant(Position* s, int32_t a) const override final { s->set_y(a); }
        void set_inv_relevant(Position* s, int32_t a) const override final { s->set_x(a); }

        bool check_dragger_collision(Vector2 position, int32_t border) override {
            return position.in_rectangle(
                { 0, border - 5 },
                { size->get().x, 11 }
            );
        }
};

class HStack : public Stack {
    protected:
        // Ewwwww!!!
        inline RayLib::MouseCursor drag_cursor() const override final { return RayLib::MOUSE_CURSOR_RESIZE_EW; }

        inline int32_t relevant_axis(Vector2 v) const override final { return v.x; };
        inline int32_t inv_relevant_axis(Vector2 v) const override final { return v.y; };
        inline SizeStrategy relevant_strat(Size* s) const override final { return s->strategy_x; }
        virtual void set_relevant(Size* s, int32_t a) const override final { s->set_x(a); }
        virtual void set_inv_relevant(Size* s, int32_t a) const override final { s->set_y(a); }
        virtual void set_relevant(Position* s, int32_t a) const override final { s->set_x(a); }
        virtual void set_inv_relevant(Position* s, int32_t a) const override final { s->set_y(a); }

        bool check_dragger_collision(Vector2 position, int32_t border) override {
            return position.in_rectangle(
                { border - 5, 0 },
                { 11, size->get().y }
            );
        }
};
