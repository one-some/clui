#pragma once

#include <functional>
#include "UI/Container/Container.h"

class Event {
public:
    // TODO: Pick one
    void stop_propagation() { do_propagate = false; }
    void prevent_default() { do_default = false; }

    bool do_propagate = true;
    bool do_default = true;

    void reset() {
        do_default = true;
    }
};

class ClickEvent : public Event { };