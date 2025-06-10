#pragma once

#include <functional>
#include "UI/Container/Container.h"

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

class ClickEvent : public Event { };

class MouseMotionEvent : public Event {
public:
    int32_t x;
    int32_t y;

    MouseMotionEvent(int32_t x, int32_t y) : x(x), y(y) { }
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