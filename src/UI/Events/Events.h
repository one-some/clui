#pragma once

#include <functional>
#include "vector2.h"

// FIXME:
class Container;

class Event {
public:
    virtual ~Event() = default;

    void stop_propagation() { do_propagate = false; }
    void prevent_default() { do_default = false; }

    bool do_propagate = true;
    bool do_default = true;

    void reset() {
        do_propagate = true;
        do_default = true;
    }
};

enum class MouseButton {
    LEFT,
    RIGHT,
    MIDDLE
};

class MouseDownEvent : public Event {
public:
    MouseButton button;

    MouseDownEvent(MouseButton button) : button(button) { }
};

class MouseUpEvent : public Event {
public:
    MouseButton button;

    MouseUpEvent(MouseButton button) : button(button) { }
};

class MouseMoveEvent : public Event {
public:
    int32_t x;
    int32_t y;

    MouseMoveEvent(int32_t x, int32_t y) : x(x), y(y) { }
};

class MouseHoverChangeEvent : public Event {
public:
    bool currently_hovering;

    MouseHoverChangeEvent(bool currently_hovering) : currently_hovering(currently_hovering) { }
};

class WheelEvent : public Event {
public:
    int32_t delta_x;
    int32_t delta_y;

    WheelEvent(int32_t delta_x, int32_t delta_y) : delta_x(delta_x), delta_y(delta_y) { }
};

class AddChildEvent : public Event {
public:
    Container* child;

    AddChildEvent(Container* child) : child(child) { }
};

class TabFocusEvent : public Event { };

class MouseHoverEvent : public Event {
public:
    // TODO: make it MS
    static const int32_t hover_time_frames;
    static int32_t hover_start_frame;
    // FIXME: Should this even be here rofl
    static Vector2 old_position;
    static bool done;

    static void update_mouse_pos(Vector2 pos, int32_t frame_no) {
        if (pos == old_position) return;
        done = false;
        old_position = pos;
        hover_start_frame = frame_no;
    }

    static bool should_propagate(int32_t frame_no) {
        if (done) return false;
        if (frame_no - hover_start_frame < hover_time_frames) return false;
        done = true;
        return true;
    }
};

#include "UI/Container/Container.h"